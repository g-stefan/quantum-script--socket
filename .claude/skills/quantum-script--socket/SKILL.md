---
name: quantum-script--socket
description: >-
  How to use the Quantum Script Socket extension (quantum-script--socket),
  the blocking TCP socket type loaded with Script.requireExtension("Socket"):
  new Socket() / Socket(), Socket.isSocket, openClient(address) (resolve +
  connect, no timeout), openServer(address) (bind only) + listen(count) +
  accept(), address text ("host:port", "[::1]:port", "0.0.0.0:port",
  "[::]:port" dual stack, URLs with default ports), read(size) / readLn(size)
  (default 32768, undefined at end, lines keep "\r\n"), write / writeLn
  (bytes sent), readToBuffer(buffer, ln) / writeFromBuffer(buffer) with
  Buffer, waitToRead / waitToWrite(microSec) (1 / 0 / -1), shutdown, close;
  recipes (HTTP GET, exact reads, length-prefixed messages, timeouts,
  stoppable server, file transfer, loopback tests); the C++ side
  (VariableSocket, value is XYO::Networking::Socket,
  registerInternalExtension). Use when writing or reviewing Quantum Script,
  magnet or extension .js code that opens network connections or calls
  Socket, C++ code that includes <XYO/QuantumScript.Extension/Socket.hpp>,
  a fabricare.json depending on "quantum-script--socket", or when working
  inside the quantum-script--socket repository.
---

# quantum-script--socket

TCP socket extension of Quantum Script (see the `quantum-script` skill for
the language and its differences from JavaScript, `quantum-script--buffer`
for `Buffer`, and `xyo-networking` for the underlying
`XYO::Networking::Socket`; their rules apply). Purpose: **let scripts talk
TCP** — clients for HTTP / SMTP / custom protocols, small servers, test
harnesses — with one API on Windows and Linux, IPv4 and IPv6.
`quantum-script--http` is built on it.

Full documentation: `docs/` in the quantum-script--socket repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--socket\docs` on this machine):
README, getting-started, **script-api** (exact behavior of every method,
address text, edge cases), **recipes** (HTTP GET, exact reads, framing,
timeouts, stoppable server, file transfer, loopback), cpp-api, reference.
Read the matching page when you need more than this summary. When in doubt
read `source/XYO/QuantumScript.Extension/Socket/Library.cpp` (~360 lines).

## Script API

```javascript
Script.requireExtension("Socket");          // also loads Buffer

var s = new Socket();                       // or Socket(); not open yet
Socket.isSocket(x);                         // true for any socket, open or not

s.openClient("example.com:80");             // true / false; resolve + connect, BLOCKS, no connect timeout
s.openServer("0.0.0.0:8080");               // true / false; bind ONLY
s.listen(16);                               // true / false; count REQUIRED (listen() -> false)
var peer = s.accept();                      // new Socket, or undefined; BLOCKS

s.write(str);                               // bytes sent; < str.length = broken; 0 when closed
s.writeLn(str);                             // write(str + "\r\n"); count includes the 2 bytes
s.read(size);                               // String: >= 1 byte that has arrived, <= size (default 32768); undefined at end
s.readLn(size);                             // one line WITH "\r\n" / "\n", <= size (default 32768); undefined at end
s.readToBuffer(buffer, ln);                 // fills from 0, LOOPS until ln bytes or end; sets buffer.length; returns count
s.writeFromBuffer(buffer);                  // sends buffer.length bytes; returns count
s.waitToRead(microSec);                     // 1 ready / 0 timeout / -1 error or closed / NaN bad arg
s.waitToWrite(microSec);
s.shutdown();                               // both directions
s.close();                                  // safe twice
```

Address text: `"127.0.0.1:80"`, `"example.com:443"` (every resolved
address tried in order), `"[::1]:80"` (IPv6 needs brackets),
`"[fe80::1%3]:80"`, `"http://host/path"` (default ports http 80, https 443,
ftp 21, ssh 22, telnet 23; path ignored). Servers: `"127.0.0.1:p"` local,
`"0.0.0.0:p"` all IPv4, `"[::]:p"` all IPv6 **and** IPv4.

## Hard rules

1. **Everything blocks.** `openClient`, `accept`, `read`, `readLn`,
   `readToBuffer` wait for the network. Put `waitToRead(timeout)` before
   them in any loop that must stop or time out; on a listening socket `1`
   means `accept` will not block.
2. **`openServer` does not listen.** Always `openServer(addr) && listen(n)`;
   `n` is required (missing / NaN / negative -> `false`).
3. **The port is required** (`"127.0.0.1"` fails) unless the text is a URL
   with a known scheme. No host (`":8080"`) fails.
4. **End of stream is `undefined`** for `read` / `readLn` and `0` for
   `readToBuffer`. Loop with `while (Script.isString(x = s.readLn(n)))`.
   Peer closed, error and "not open" are not told apart.
5. **Lines keep their ending.** An empty protocol line is `"\r\n"` (or
   `"\n"`), never `""`; end of HTTP headers: `line == "\r\n"`. Use
   `.trim()` for the text. A lone `"\r"` stays inside the line. Lines
   longer than `size` come in pieces.
6. **`readLn` reads one byte per system call on purpose**: it never takes
   the bytes after the line (body stays for `read` / `readToBuffer`), but is
   slow for bulk data.
7. **`read` vs `readToBuffer`.** `read(size)` returns what has arrived
   (TCP has no message boundaries; loop to collect). `readToBuffer(b, ln)`
   waits for **all** `ln` bytes or the close: with a large buffer and a
   short answer it hangs until the peer closes. Pass the exact expected
   count. It never reads past `b.size` (resize first) and throws
   `invalid parameter` if `b` is not a Buffer.
8. **Pass `ln` to `readToBuffer` explicitly.** Missing `ln` means 32768 in
   current source, but builds up to 7.9.0 build 12 read nothing and return
   `0`. `ln` 0 / negative / NaN: length 0, returns 0; `Infinity`: `b.size`.
9. **Writes report failure by count.** Compare `write` /
   `writeFromBuffer` results with the length; the first write after the
   peer closed usually still "succeeds". Network errors never throw.
10. **`if (s)` is always true**, even closed; there is no `isOpen` and no
    peer address in scripts. No own properties (`s.x = 1` throws);
    `Socket.prototype` can be extended.
11. **Not supported:** connect timeout, TLS (an `https://` URL is plain TCP
    to port 443), UDP, socket options, half close (`shutdown` stops both
    directions), non-blocking / events.
12. **One thread owns a socket.** Each thread requires `Socket` itself; a
    socket cannot be passed to another thread (no `clone`). Open it where
    it is used.
13. **Strings are bytes**: no transcoding; `str.length` is the byte count,
    the same unit `write` returns.
14. `waitToRead` / `waitToWrite` take **microseconds** (`1000000` = 1 s,
    max ~71 min, `0` = poll). `waitToRead` returns `1` when the peer
    closed too; the read after it returns the end of stream.
15. Close sockets explicitly; collection closes them too, but only when it
    runs.
16. Quantum Script syntax: `typeof(x)` with parentheses, no `!!x` (use
    `Convert.toBoolean(x)`), `var` only, statements end with `;` also after
    blocks (`};`).

## Patterns

```javascript
// client with headers + body
s.writeLn("GET / HTTP/1.0"); s.writeLn("Host: example.com"); s.writeLn("Connection: close"); s.writeLn("");
while (Script.isString(line = s.readLn(8192)) && line != "\r\n") { /* header */ };
while (Script.isString(data = s.read(32768))) { body += data; };

// timeout
if (s.waitToRead(5 * 1000000) == 1) { line = s.readLn(8192); };

// stoppable server loop
while (running) {
	if (server.waitToRead(100000) != 1) { continue; };
	peer = server.accept();
	if (Socket.isSocket(peer)) { serve(peer); peer.close(); };
};

// framed binary: exact counts with readToBuffer
if (s.readToBuffer(b, 4) == 4) { n = b.getU8(0) * 16777216 + b.getU8(1) * 65536 + b.getU8(2) * 256 + b.getU8(3); };

// tests: client and server in ONE script work (connect completes in the backlog)
server.openServer("127.0.0.1:47000") && server.listen(4);
client.openClient("127.0.0.1:47000"); peer = server.accept();
```

Never wait in a single-thread loopback for data the same script sends
later: it blocks forever. Port 0 cannot be read back: try a port range.

## C++

```cpp
#include <XYO/QuantumScript.Extension/Socket.hpp>
#include <XYO/QuantumScript.Extension/Socket/VariableSocket.hpp>
using namespace XYO::QuantumScript;
using Extension::Socket::VariableSocket;

Extension::Buffer::registerInternalExtension(executive);   // Socket requires Buffer
Extension::Socket::registerInternalExtension(executive);   // host init callback; scripts still requireExtension

// argument
if (!TIsType<VariableSocket>(arguments->index(0))) { throw(Error("invalid parameter")); };
XYO::Networking::Socket &socket = ((VariableSocket *)(arguments->index(0)).value())->value;
// socket.getIPAddress4 / getIPAddress6, read / write, Stream::readLn(socket, out, size), ...

// return
TPointer<Variable> r(VariableSocket::newVariable());        // closed socket
if (((VariableSocket *)r.value())->value.openClientX(address)) { return r; };
return Context::getValueUndefined();
```

- Another extension using sockets: depend on `"quantum-script--socket"` in
  `fabricare.json` and run
  `executive->compileStringX("Script.requireExtension(\"Socket\");");` in
  its `initExecutive` before anything creates sockets.
- Static builds have no `quantumScriptExtension` entry point
  (`XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_LIBRARY`): register as internal.
- `magnet` registers `Socket` as an internal extension.

## Working in this repository

- Build: `fabricare make`, `fabricare install` (see the `fabricare` skill).
  `quantum-script`, `quantum-script--console`, `quantum-script--buffer` and
  `xyo-networking` must be installed first. There is no `test/` project;
  test with a loopback script run by `quantum-script` (see
  `docs/recipes.md`).
- Native methods live in `Socket/Library.cpp` as
  `static TPointer<Variable> name(VariableFunction *, Variable *this_, VariableArray *arguments)`,
  check `TIsType<VariableSocket>(this_)` first, and are registered in
  `initExecutive` with
  `executive->setFunction2("Socket.prototype.name(args)", name)`; the
  argument list in that string must match what the function reads.
- New methods: update `README.md`, `docs/script-api.md`,
  `docs/reference.md` and this skill.
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX header: MIT for `source/` and `docs/`,
  Unlicense for `.claude/` (see `.reuse/dep5`).
