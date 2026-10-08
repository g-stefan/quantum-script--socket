# Recipes

Every recipe starts with:

```javascript
Script.requireExtension("Console");
Script.requireExtension("Socket");    // also loads Buffer
```

## HTTP GET

A plain HTTP/1.0 request: headers with `readLn`, then the body with `read`
until the server closes or `Content-Length` bytes have arrived.

```javascript
// host: "example.com" or "example.com:8080" (an IPv6 host needs brackets and a port)
function httpGet(host, path) {
	var address = host;
	if (address.indexOf(":") < 0) {
		address += ":80";
	};
	var socket = new Socket();
	if (!socket.openClient(address)) {
		return undefined;
	};
	socket.writeLn("GET " + path + " HTTP/1.0");
	socket.writeLn("Host: " + host);
	socket.writeLn("Connection: close");
	socket.writeLn("");

	var status = socket.readLn(8192);
	if (Script.isUndefined(status)) {
		socket.close();
		return undefined;
	};
	var headers = {};
	var line;
	var x;
	while (Script.isString(line = socket.readLn(8192))) {
		if (line == "\r\n" || line == "\n") {
			break;                                   // end of the headers
		};
		x = line.indexOf(":");
		if (x > 0) {
			headers[line.substring(0, x).toLowerCaseASCII()] = line.substring(x + 1).trim();
		};
	};

	var contentLength = -1;
	if (Script.isString(headers["content-length"])) {
		contentLength = Convert.toNumber(headers["content-length"]);
	};
	var body = "";
	var data;
	while (contentLength < 0 || body.length < contentLength) {
		data = socket.read(32768);
		if (Script.isUndefined(data)) {
			break;                                   // server closed
		};
		body += data;
	};
	socket.close();
	return {status: status.trim(), headers: headers, body: body};
};

var response = httpGet("example.com", "/");
if (response) {
	Console.writeLn(response.status);               // HTTP/1.0 200 OK
	Console.writeLn(response.body.length + " bytes");
};
```

There is no TLS: `https` servers cannot be reached this way. For real HTTP
work use the `quantum-script--http` extension, built on `Socket`.

## Read an exact number of bytes

`read` returns what has arrived; loop until the count is reached:

```javascript
function readExact(socket, size) {
	var out = "";
	var data;
	while (out.length < size) {
		data = socket.read(size - out.length);
		if (Script.isUndefined(data)) {
			return undefined;                        // closed before the end
		};
		out += data;
	};
	return out;
};
```

With a buffer, `readToBuffer` does the loop itself:

```javascript
var b = Buffer(1024);
if (socket.readToBuffer(b, 1024) != 1024) {
	// closed before 1024 bytes arrived
};
```

## Length-prefixed messages

A binary framing: 4 bytes of length (big endian), then the bytes of the
message. Unlike lines, the message may contain anything.

```javascript
function sendMessage(socket, text) {
	var body = Buffer.fromString(text);
	var n = body.length;
	var head = Buffer(4);
	head.setU8(0, (n >> 24) & 0xFF);
	head.setU8(1, (n >> 16) & 0xFF);
	head.setU8(2, (n >> 8) & 0xFF);
	head.setU8(3, n & 0xFF);
	if (socket.writeFromBuffer(head) != 4) {
		return false;
	};
	return socket.writeFromBuffer(body) == n;
};

function receiveMessage(socket, buffer) {         // buffer: a reusable Buffer
	if (buffer.size < 4) {
		buffer.resize(4);                            // readToBuffer never reads past size
	};
	if (socket.readToBuffer(buffer, 4) != 4) {
		return undefined;                            // closed
	};
	var n = buffer.getU8(0) * 16777216 + buffer.getU8(1) * 65536 + buffer.getU8(2) * 256 + buffer.getU8(3);
	if (n > buffer.size) {
		buffer.resize(n);
	};
	if (socket.readToBuffer(buffer, n) != n) {
		return undefined;
	};
	return buffer.toString();
};
```

`readToBuffer` with the exact count is the right tool here: it waits for
the whole frame and never takes bytes of the next one.

## Wait with a timeout

`waitToRead` before a blocking call turns it into a call with a timeout:

```javascript
// one line, or undefined after microSec without data (or when closed)
function readLnTimeout(socket, size, microSec) {
	if (socket.waitToRead(microSec) != 1) {
		return undefined;
	};
	return socket.readLn(size);
};

var line = readLnTimeout(socket, 8192, 5 * 1000000);   // 5 seconds
```

`1` means *some* data arrived: if only half a line is there, `readLn` still
waits for the rest. For strict deadlines read with `read` and split the
lines yourself.

## A server that can stop

`accept` and `readLn` block; a loop around `waitToRead` keeps control, so
the server can stop on a command and drop idle clients.

```javascript
var server = new Socket();
if (!(server.openServer("127.0.0.1:8080") && server.listen(16))) {
	throw "cannot listen";
};

var running = true;
var peer;
var line;
var idle;
while (running) {
	if (server.waitToRead(100 * 1000) != 1) {   // 100 ms
		continue;                                 // timeout: check timers, flags, ...
	};
	peer = server.accept();
	if (!Socket.isSocket(peer)) {
		continue;
	};
	idle = 0;
	while (idle < 300) {                          // 300 x 100 ms = 30 s without data
		if (peer.waitToRead(100 * 1000) != 1) {
			++idle;
			continue;
		};
		idle = 0;
		line = peer.readLn(8192);
		if (Script.isUndefined(line)) {
			break;                                // client closed
		};
		line = line.trim();
		if (line == "stop") {
			running = false;
			break;
		};
		peer.writeLn("you said: " + line);
	};
	peer.close();
};
server.close();
```

It serves one client at a time. To serve several clients at once, run the
same loop in several script threads (`Thread` extension), each with its own
listening socket on a different port, or one thread per connection that
opens its own client socket — a `Socket` cannot be handed to another thread.

## Send and receive a file

```javascript
Script.requireExtension("File");

function sendFile(socket, fileName) {
	var file = new File();
	if (!file.openReadOnly(fileName)) {
		return false;
	};
	var chunk = Buffer(65536);
	var ok = true;
	while (file.readToBuffer(chunk, chunk.size) > 0) {
		if (socket.writeFromBuffer(chunk) != chunk.length) {
			ok = false;                              // connection broken
			break;
		};
	};
	file.close();
	return ok;
};

function receiveFile(socket, fileName, size) {    // size known from the protocol
	var file = new File();
	if (!file.openWrite(fileName)) {
		return false;
	};
	var chunk = Buffer(65536);
	var want;
	while (size > 0) {
		want = size;
		if (want > chunk.size) {
			want = chunk.size;
		};
		if (socket.readToBuffer(chunk, want) != want) {
			break;                                   // closed early
		};
		file.writeFromBuffer(chunk);
		size -= want;
	};
	file.close();
	return size == 0;
};
```

## Find a free port

Port `0` cannot be read back, so tests try a range:

```javascript
function listenOnFreePort(host, first, last) {
	var server = new Socket();
	var port;
	for (port = first; port <= last; ++port) {
		if (server.openServer(host + ":" + port) && server.listen(16)) {
			return {socket: server, port: port};
		};
	};
	return undefined;
};

var s = listenOnFreePort("127.0.0.1", 47000, 47100);
```

## Client and server in one script

A connection is complete as soon as the server listens, before `accept`,
and small writes fit in the system buffers. So a single script can talk to
itself, which is handy for tests:

```javascript
var s = listenOnFreePort("127.0.0.1", 47000, 47100);
var client = new Socket();
client.openClient("127.0.0.1:" + s.port);
var peer = s.socket.accept();          // returns at once: the client is already connected

client.writeLn("ping");
Console.write(peer.readLn());           // ping
peer.writeLn("pong");
Console.write(client.readLn());         // pong

client.close();
Script.isUndefined(peer.read());        // true: the client closed
peer.close();
s.socket.close();
```

Do not wait in this pattern for something only the other side of the same
script will send later: the call blocks forever.
