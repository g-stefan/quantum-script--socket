# Script API

Everything the extension defines after `Script.requireExtension("Socket")`.
All calls are **blocking** unless noted: the script waits in the call until
the operating system answers. Use `waitToRead` / `waitToWrite` first when
the script must not wait forever.

Methods called on something that is not a socket
(`Socket.prototype.read.call({})`) throw `Error: invalid parameter`.

"Number conversion" below means the argument goes through `toNumber`:
`undefined` and non numeric strings become `NaN`, `null` becomes `0`,
fractions are truncated when the value is used as a count.

## Constructor

### `Socket()` / `new Socket()`

Returns a new socket, **not open**. It becomes usable after a successful
`openClient`, `openServer` (+ `listen`), or when it is returned by
`accept`. Both forms give the same result.

```javascript
var s = new Socket();
typeof(s);            // "Socket"
s instanceof Socket;  // true
```

## Static functions

### `Socket.isSocket(x)`

`true` when `x` is a socket (open or not), `false` otherwise. Use it to
test the result of `accept()`.

## Opening

### `openClient(address)`

Resolves `address` (converted to a string) and **connects**. Returns
`true` when connected, `false` on any failure (bad address text, name not
resolved, connection refused, unreachable host).

- Blocks until the connection is made or fails. There is **no connect
  timeout**: the operating system default applies (immediate for
  "refused", up to minutes for an unreachable host).
- A name that resolves to several addresses is tried address by address
  until one connects: `"localhost:8080"` reaches a server on `127.0.0.1`
  even when `localhost` resolves to `::1` first.
- If the socket was already open, it is closed first, so one `Socket` can
  be reused for several connections.

### `openServer(address)`

Resolves `address` and **binds** the socket to it. Returns `true` /
`false` (port in use, no permission, bad address). It does **not** listen:
call `listen(count)` next.

- `"127.0.0.1:port"`: local clients only. `"0.0.0.0:port"`: every IPv4
  interface. `"[::]:port"`: every IPv6 and IPv4 interface — IPv6 servers are
  dual stack on Windows and Linux.
- Linux sets `SO_REUSEADDR` (a restarted server can bind again at once);
  Windows does not (it would let another process take the port).
- There is no way to read back the port chosen by the system for port `0`;
  pick a port, or try a range (see [Recipes](recipes.md#find-a-free-port)).

### Address text

Both functions take the same text forms:

| Text | Host | Port |
|------|------|------|
| `"127.0.0.1:8080"` | `127.0.0.1` | `8080` |
| `"example.com:443"` | resolved, IPv4 or IPv6 | `443` |
| `"[::1]:8080"` | `::1` | `8080` |
| `"[fe80::1%3]:8080"` | link-local `fe80::1` on interface index `3` (`%eth0` also works on Linux) | `8080` |
| `"0.0.0.0:8080"`, `"[::]:8080"` | every interface (servers) | `8080` |
| `"http://example.com/path?q=1"` | `example.com` | `80`, from the scheme |
| `"https://user:pw@[::1]/"` | `::1` (user info skipped) | `443` |

- The **port is required** unless the text is a URL whose scheme has a
  known port: `http` 80, `https` 443, `ftp` 21, `ssh` 22, `telnet` 23.
  `"127.0.0.1"` alone fails.
- The port is decimal, at most `65535`. The host is required (`":8080"`
  fails).
- IPv6 addresses with a port need brackets.
- A URL only selects host and port: path, query and user info are ignored,
  and the connection is plain TCP (no TLS for `https`).

## Server

### `listen(count)`

Starts listening on a socket opened with `openServer`. `count` is the
backlog: how many connections may wait for `accept`. Returns `true` /
`false`.

- **`count` is required**: `listen()` converts `undefined` to `NaN` and
  returns `false`. `NaN`, `±Infinity` and negative values return `false`.
- Use a value between `1` and `65535` (it is stored as a 16 bit number);
  `16` .. `128` is typical.

### `accept()`

Waits for the next client and returns a **new `Socket`** for that
connection, or `undefined` if the server is not open / not listening or the
call failed.

- Blocks until a client connects. To wait with a timeout, call
  `waitToRead` first: on a listening socket it returns `1` when a
  connection is ready, and the following `accept` does not block.
- The returned socket is independent of the server: close it when the
  conversation ends, the server keeps listening.

```javascript
var peer = server.accept();
if (Socket.isSocket(peer)) {
	// serve peer
	peer.close();
};
```

## Text

Quantum Script strings are bytes (UTF-8 by convention): what is written is
sent byte for byte, what is read is returned byte for byte, with no
transcoding. Counts are bytes.

### `write(str)`

Sends `str` (converted with `toString`). Returns the number of bytes sent.

- Sends everything: it loops until all bytes are accepted by the system.
  **A result smaller than `str.length` means the connection is broken.**
- Returns `0` on a closed socket or after `shutdown`.
- TCP reports a closed peer late: the first `write` after the peer closed
  usually still succeeds (the bytes are lost); later ones fail.
- On Linux a write to a closed connection fails instead of killing the
  process with `SIGPIPE`.

### `writeLn(str)`

`write(str + "\r\n")`. Returns the bytes sent, **including the 2 bytes of
`"\r\n"`**: `writeLn("abc")` returns `5` on success.

### `read(size)`

Receives text: waits until **at least one byte** has arrived, then returns
what is there, up to `size` bytes, as a String. Returns `undefined` when
nothing can be read: the peer closed the connection, an error happened, or
the socket is not open (the cases are not told apart).

| `size` | Result |
|--------|--------|
| missing (`undefined`) | up to **32768** bytes |
| a positive number | up to `size` bytes (fraction truncated) |
| `0`, `null` | `""` at once, nothing is read |
| negative, `NaN`, `±Infinity` | `undefined`, nothing is read |

- TCP is a byte stream, not messages: one `read` may return part of what the
  peer sent with one `write`, or several writes joined. Read in a loop and
  split the data with your protocol (lines, a length prefix, ...).
- `read` returns what has arrived, not necessarily `size` bytes; it may
  also wait for more while the bytes received so far fill its internal
  blocks exactly. For an exact count use `readToBuffer` (see below) or a
  loop (see [Recipes](recipes.md#read-an-exact-number-of-bytes)).

### `readLn(size)`

Receives **one line**: the bytes up to and including the first `"\n"`.
Returns the line **with its line ending** (`"\r\n"` or `"\n"`), or
`undefined` at the end of the stream.

- At most `size` bytes (default **32768**); a longer line comes back in
  pieces, the next call continues it.
- A last line without a line ending is returned as is; the call after it
  returns `undefined`.
- A lone `"\r"` is kept inside the line; it does not end it.
- An empty line of the protocol comes back as `"\r\n"` (or `"\n"`), never
  as `""`: compare with `"\r\n"` to find the end of HTTP / SMTP headers.
- `readLn` reads **one byte per system call on purpose**: it never takes
  bytes after the line, so the data that follows (an HTTP body, a binary
  payload) is still in the socket for `read` / `readToBuffer`. It is slow
  for bulk data: switch to `read` / `readToBuffer` after the header lines.
- `size` follows the same rules as for `read`: `0` / `null` return `""`,
  negative / `NaN` / `±Infinity` return `undefined`.

```javascript
var line = socket.readLn(8192);
if (Script.isUndefined(line)) {
	// connection closed
};
var text = line.trim();   // without "\r\n"
```

## Binary data

These two methods use the `Buffer` type of `quantum-script--buffer` (loaded
by `Socket`). A buffer has a capacity (`size`) and a used part (`length`);
see the `quantum-script--buffer` documentation.

### `readToBuffer(buffer, ln)`

Fills `buffer` from index 0 with up to `ln` bytes and sets `buffer.length`
to the count. Returns the number of bytes read.

| `ln` | Bytes requested |
|------|-----------------|
| missing | 32768, clipped to `buffer.size` |
| a number | `ln`, clipped to `buffer.size` |
| `Infinity` | `buffer.size` |
| `0`, negative, `NaN` | nothing is read: `buffer.length` becomes `0`, returns `0` |

- **It loops until `ln` bytes have arrived or the peer closes.** Unlike
  `read`, it does not return early with what has arrived: with a 64 KB
  buffer and a 10 byte answer, `readToBuffer(b, b.size)` waits until the
  server closes the connection. Pass the number of bytes you expect
  (a `Content-Length`, a length prefix), or use `read` for "whatever is
  there".
- Returns `0` (and `length` 0) when the connection is already closed: the
  end of the stream.
- Throws `Error: invalid parameter` when `buffer` is not a `Buffer`.
- The buffer never grows: `resize` it first for a larger read.
- Builds up to version 7.9.0 (build 12) ignore a missing `ln` and read
  nothing (return `0`). Pass `ln` explicitly when the script may run with
  an older `quantum-script--socket` library.

### `writeFromBuffer(buffer)`

Sends the first `buffer.length` bytes of `buffer`. Returns the number of
bytes sent; less than `buffer.length` means the connection is broken.
Throws `Error: invalid parameter` when `buffer` is not a `Buffer`.

```javascript
var chunk = Buffer(65536);
var n = socket.readToBuffer(chunk, 1024);   // exactly 1024 bytes, or fewer at the end
other.writeFromBuffer(chunk);               // sends chunk.length == n bytes
```

## Waiting

### `waitToRead(microSec)`

Waits up to `microSec` microseconds until the socket can be read without
blocking. Returns:

| Result | Meaning |
|--------|---------|
| `1` | ready: data has arrived, the peer closed (the next read returns `undefined` / `0`), or — on a listening socket — a client is waiting for `accept` |
| `0` | timeout |
| `-1` | error, or the socket is not open |
| `NaN` | `microSec` missing, negative, `NaN` or `±Infinity` (nothing is waited) |

- `0` checks without waiting. `1000000` is one second. The maximum is
  `4294967295` (about 71 minutes): wait in a loop for longer.
- `1` does not mean a whole line or message is there: `readLn` and
  `readToBuffer` may still block for the rest of it.

### `waitToWrite(microSec)`

Same results, for writing: `1` when there is room to send. A connected
socket is almost always writable; this matters only when the peer stops
reading and the system send buffer is full.

## Closing

### `shutdown()`

Stops sending **and** receiving (both directions); the peer sees the end
of the stream. The socket stays open until `close()`. After it, `write`
returns `0` and reads return the end of the stream. There is no half close
(stop sending but keep reading the answer): mark the end of a request in the
protocol instead (`Content-Length`, `Connection: close`, a terminator line).
Returns `undefined`.

### `close()`

Shuts the connection down and releases it. Safe to call more than once,
and on a socket that was never opened. Returns `undefined`.

A socket that is no longer referenced is closed when the engine collects
it, but close sockets explicitly: until the collection runs, the connection
(or the listening port) stays open.

## Conversions and properties

| Expression | Result |
|------------|--------|
| `typeof(s)` | `"Socket"` |
| `s instanceof Socket` | `true` |
| `Socket.isSocket(s)` | `true` |
| `"" + s`, `s.toString()` | `"Socket"` |
| `if (s)`, `Convert.toBoolean(s)` | **always `true`**, open or not |
| `s.tag = 1` | throws `setPropertyBySymbol`: no own properties |

There is no property or method that tells whether a socket is open or
whether the peer is still there: a closed connection shows up as `read` /
`readLn` returning `undefined`, `readToBuffer` returning `0`, or `write`
returning less than requested. Keep your own flag if you need one.

Methods can be added to `Socket.prototype`:

```javascript
Socket.prototype.writeLines = function(lines) {
	var k;
	for (k = 0; k < lines.length; ++k) {
		this.writeLn(lines[k]);
	};
};
```

## Not supported

| Not available | Use instead |
|---------------|-------------|
| Connect timeout | none in the API; the OS default applies. Connect from a separate thread if the script must not hang |
| Peer / local address (`remoteAddress`) | not exposed to scripts (the C++ `XYO::Networking::Socket` has `getIPAddress4/6`) |
| UDP, Unix domain sockets | none |
| TLS (`https`, `smtps`) | none in this extension: the connection is plain TCP |
| Socket options (`TCP_NODELAY`, keep-alive, buffer sizes) | none |
| Non-blocking mode, events, callbacks | `waitToRead` / `waitToWrite` with a timeout in a loop |
| Half close (`shutdown` of one direction) | a protocol level end marker |
| Passing a socket to another thread | open it in the thread that uses it |
