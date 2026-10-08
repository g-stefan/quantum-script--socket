# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `quantum-script` (and everything
below it: `xyo-system`, `xyo-encoding`, ...), `quantum-script--console`,
`quantum-script--buffer` and `xyo-networking` must be installed to the SDK
first. From the repository root:

```bash
fabricare make       # build into output/
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

The project `quantum-script--socket` is a `dll-or-lib` project:

| Build | Result | Use it when |
|-------|--------|-------------|
| dynamic (default platform, `win64-msvc-2026`, `ubuntu-*`) | `quantum-script--socket.dll` / `libquantum-script--socket.so` | scripts run by `quantum-script`, or a host using the engine DLL |
| static (`--static`, or the `win64-msvc-2026.static` platform) | `quantum-script--socket.lib` / `.a` | self-contained hosts built with the static engine |

After `fabricare install`, `quantum-script--socket.dll` (Windows) /
`libquantum-script--socket.so` (Linux) sits in the SDK `bin` folder next to
`quantum-script.exe` and `quantum-script--buffer.dll`, which is where
`Script.requireExtension("Socket")` finds it.

## 2. A first client

```javascript
Script.requireExtension("Console");
Script.requireExtension("Socket");

var socket = new Socket();
if (!socket.openClient("example.com:80")) {
	throw "cannot connect";
};
socket.writeLn("GET / HTTP/1.0");
socket.writeLn("Host: example.com");
socket.writeLn("Connection: close");
socket.writeLn("");

var line;
while (Script.isString(line = socket.readLn(8192))) {
	if (line == "\r\n") {
		break;                    // end of the headers
	};
	Console.write(line);          // line keeps its "\r\n"
};
var body = "";
var data;
while (Script.isString(data = socket.read(32768))) {
	body += data;                 // undefined when the server closes
};
socket.close();
Console.writeLn(body.length + " bytes of body");
```

Run it with:

```bash
quantum-script http-get.js
```

`Script.requireExtension("Socket")` looks for an external
`quantum-script--socket` library first (the file as named, then every
include path folder: next to the interpreter, next to the script), then for
an internal extension registered by the host. Loading twice does nothing. A
missing extension throws `Unable to open "Socket"`.

Loading `Socket` also runs `Script.requireExtension("Buffer")`, so the
`quantum-script--buffer` library must be available too, and the `Buffer`
global exists afterwards.

## 3. A first server

```javascript
Script.requireExtension("Console");
Script.requireExtension("Socket");

var server = new Socket();
if (!(server.openServer("127.0.0.1:8080") && server.listen(16))) {
	throw "cannot listen on 8080";
};
Console.writeLn("echo server on 127.0.0.1:8080");

for (;;) {
	var peer = server.accept();      // blocks until a client connects
	if (!Socket.isSocket(peer)) {
		continue;
	};
	var line;
	while (Script.isString(line = peer.readLn(8192))) {
		if (line == "quit\r\n" || line == "quit\n") {
			break;
		};
		peer.write(line);            // echo, with the client's line ending
	};
	peer.close();
};
```

Try it with `telnet 127.0.0.1 8080` or `nc 127.0.0.1 8080`. This server
serves one client at a time; see [Recipes](recipes.md) for timeouts and a
server that can stop.

- `openServer` only **binds**; `listen(n)` must follow, otherwise `accept`
  fails.
- `"127.0.0.1:port"` accepts local clients only. `"0.0.0.0:port"` listens on
  every IPv4 interface, `"[::]:port"` on every IPv6 **and** IPv4 interface
  (dual stack).
- On Linux ports below 1024 need privileges.

## 4. magnet and other hosts

`quantum-script--magnet` registers `Socket` (with `Buffer`, `HTTP`, ...) as
an internal extension, so scripts run by `magnet` use it without the DLL.
`quantum-script--http` requires `Socket` itself: after
`Script.requireExtension("HTTP")`, `Socket` exists too.

## 5. Register it in a C++ host

A host that embeds Quantum Script makes `Socket` available as an internal
extension by registering it, together with `Buffer`, in the init callback:

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/Socket.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);   // Socket requires Buffer
	Extension::Socket::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (!ExecutiveX::executeFile("main.js")) {
			printf("%s\n", (ExecutiveX::getError()).value());
			printf("%s", (ExecutiveX::getStackTrace()).value());
		};
		ExecutiveX::endProcessing();
	};
	return 0;
};
```

Registering only makes the extension *available*: scripts still call
`Script.requireExtension("Socket")`. With the DLL build of the engine an
external `quantum-script--socket.dll` found on the include path wins over
the internal one for `requireExtension`; use
`Script.requireInternalExtension("Socket")` to force the internal one.

In the host's `fabricare.json`:

```json
{
	"name": "my-host",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script--socket"
	]
}
```

`quantum-script--socket` brings `quantum-script`, `quantum-script--console`,
`quantum-script--buffer` and `xyo-networking` with it.

## 6. Static builds

A static build of the extension has no `quantumScriptExtension` DLL entry
point: a static host must register `Socket` (and `Buffer`) with
`registerInternalExtension` (section 5). External DLLs cannot be loaded into
a host that does not use the engine DLL.

When the extension is compiled into another library, define
`XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_LIBRARY`: it turns
`XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT` into nothing and leaves out the
entry point.

## 7. Threads

Each thread that runs scripts has its own engine and its own `Socket`
prototype (`SocketContext` is a per thread singleton), so every thread loads
the extension itself with `Script.requireExtension("Socket")`. A `Socket`
object belongs to the thread that created it and **cannot be passed to
another thread** (it is not cloneable: a copy made by the `Thread`
extension is not a socket). Open the connection in the thread that uses it,
or pass the address and connect there.

A thread blocked in `accept`, `read`, `readLn` or `readToBuffer` does not
wake up by itself: use `waitToRead` with a timeout in every loop that must
stop on request.
