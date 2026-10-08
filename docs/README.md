# Quantum Script Extension Socket — Documentation

`quantum-script--socket` is the **TCP networking extension of Quantum
Script**. Loaded with `Script.requireExtension("Socket")`, it adds a
`Socket` type to scripts: a blocking TCP connection, IPv4 or IPv6, that can
connect to a server (client) or bind, listen and accept connections
(server), and exchange text or binary data.

It is a thin script binding of `XYO::Networking::Socket` from the
`xyo-networking` library: every method maps to one C++ call, so the
behavior (address text, blocking, timeouts, platform differences) is the
one documented in the `xyo-networking` repository, `docs/sockets.md`.

- **Client.** `openClient("host:port")` resolves the name and connects,
  blocking until the connection is made or fails.
- **Server.** `openServer("0.0.0.0:port")` binds, `listen(n)` starts
  listening, `accept()` waits for a client and returns a new `Socket` for
  that connection.
- **Text.** `write(str)` / `writeLn(str)` send strings (`writeLn` adds
  `"\r\n"`); `read(size)` returns what has arrived, `readLn(size)` one line,
  with its line ending.
- **Binary.** `readToBuffer(buffer, ln)` / `writeFromBuffer(buffer)` move
  bytes through a `Buffer` (the `quantum-script--buffer` extension, loaded
  automatically).
- **Timeouts.** `waitToRead(microSec)` / `waitToWrite(microSec)` wait until
  the socket is ready, so a loop can check other things instead of blocking
  forever.

```
scripts: quantum-script .js, fabricare / magnet build scripts, quantum-script--http
quantum-script--socket    <-- this extension: the Socket type
quantum-script--buffer    (Buffer, for readToBuffer / writeFromBuffer)
quantum-script            (Executive, Variable, Context)
xyo-networking            (XYO::Networking::Socket: address parsing, TCP, IPv4 / IPv6)
xyo-system                (Stream::read / readLn / readToBuffer)
xyo-encoding, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What `Socket` gives |
|------|---------------------|
| Talk to a TCP service from a script (HTTP, SMTP, Redis, a custom protocol) | `openClient`, `writeLn`, `readLn`, `read` |
| A small server in a script (test server, local tool, control port) | `openServer`, `listen`, `accept` |
| Line based protocols | `readLn` reads one byte at a time on purpose: it never takes bytes after the line, so the body stays in the socket for the next `read` / `readToBuffer` |
| Binary data, file transfer | `readToBuffer` / `writeFromBuffer` with a reusable `Buffer` |
| Loops that must stop (idle timeout, stop flag) | `waitToRead` / `waitToWrite` with a timeout in microseconds |
| One API on Windows and Linux, IPv4 and IPv6 | the address is text: `"127.0.0.1:80"`, `"[::1]:80"`, `"example.com:443"`, `"http://example.com/"` |

`quantum-script--http` is built on this extension.

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("Socket");` | also loads `Buffer` |
| New socket | `var s = new Socket();` or `Socket()` | closed until an `open*` / `accept` succeeds |
| Connect | `s.openClient("example.com:80")` | `true` / `false`; blocks, no connect timeout |
| Serve | `s.openServer("0.0.0.0:8080") && s.listen(16)` | `openServer` binds only; `listen` is required |
| Next client | `var peer = s.accept();` | a new `Socket`, or `undefined`; blocks |
| Send text | `s.write(str)`, `s.writeLn(str)` | returns bytes sent; less than the length means the connection is broken |
| Receive text | `s.read(size)`, `s.readLn(size)` | a String, or `undefined` when the peer closed; default size 32768 |
| Send / receive bytes | `s.writeFromBuffer(b)`, `s.readToBuffer(b, ln)` | `readToBuffer` loops until `ln` bytes or the end |
| Wait | `s.waitToRead(100000)` | `1` ready, `0` timeout, `-1` error / closed |
| End | `s.close()` | safe to call twice; also closed when the object is collected |
| Type test | `Socket.isSocket(x)`, `typeof(x) == "Socket"` | |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, a first client and server, fabricare / magnet scripts, register it in a C++ host, static builds, threads |
| [Script API](script-api.md) | Every function: arguments, exact behavior, return values, edge cases, address text |
| [Recipes](recipes.md) | HTTP request, line protocol server, timeouts, exact-length reads, binary transfer, port scanning for tests |
| [C++ API](cpp-api.md) | `VariableSocket`, `registerInternalExtension`, using sockets from your own native functions |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding,
writing extensions) is documented in the `quantum-script` repository,
`docs/`; the `Buffer` type in the `quantum-script--buffer` repository,
`docs/`; the underlying socket in the `xyo-networking` repository, `docs/`.

## Source map

```
source/XYO/QuantumScript.Extension/Socket.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/Socket.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/Socket/
    Dependency.hpp                                       <XYO/QuantumScript.hpp>, <XYO/Networking.hpp>, export macro
    Library[.hpp/.cpp]                                   initExecutive, registerInternalExtension,
                                                         Socket() and every native function
    Context.hpp                                          SocketContext: the Socket prototype per thread
    VariableSocket[.hpp/.cpp]                            the script value type (wraps XYO::Networking::Socket)
    Copyright / License / Version                        library metadata
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--socket/`](../.claude/skills/quantum-script--socket/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use `Socket`
(Quantum Script tools, fabricare scripts, other extensions).
