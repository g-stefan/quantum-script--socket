# API reference

## Script

Available after `Script.requireExtension("Socket")` (which also loads
`Buffer`).

### Global

| Symbol | Returns | Notes |
|--------|---------|-------|
| `Socket()` / `new Socket()` | Socket | new, not open |
| `Socket.isSocket(x)` | Boolean | `x` is a socket |

### Methods

| Method | Returns | Behavior |
|--------|---------|----------|
| `s.openClient(address)` | Boolean | resolve and connect; blocks, no timeout |
| `s.openServer(address)` | Boolean | resolve and bind (no listen) |
| `s.listen(count)` | Boolean | start listening; `count` required, `0 .. 65535` |
| `s.accept()` | Socket / `undefined` | next client; blocks |
| `s.read(size)` | String / `undefined` | what has arrived, at least 1 byte, at most `size` (default 32768); `undefined` at end |
| `s.readLn(size)` | String / `undefined` | one line with its `"\r\n"` / `"\n"`, at most `size` (default 32768); `undefined` at end |
| `s.write(str)` | Number | bytes sent; less than `str.length` = broken |
| `s.writeLn(str)` | Number | `write(str + "\r\n")`, count includes the 2 bytes |
| `s.readToBuffer(buffer, ln)` | Number | fills `buffer` from 0 until `ln` bytes (default 32768, clipped to `size`) or end; sets `length` |
| `s.writeFromBuffer(buffer)` | Number | sends `buffer.length` bytes |
| `s.waitToRead(microSec)` | Number | `1` ready, `0` timeout, `-1` error / closed, `NaN` bad argument |
| `s.waitToWrite(microSec)` | Number | same, for writing |
| `s.shutdown()` | `undefined` | stop both directions |
| `s.close()` | `undefined` | close; safe twice |

`size` / `ln`: `0` (and `null` for `read` / `readLn`) reads nothing;
negative, `NaN` give `undefined` (`read`, `readLn`) or `0` (`readToBuffer`);
`Infinity` gives `undefined` for `read` / `readLn` and `buffer.size` for
`readToBuffer`.

### Address text

`"host:port"`, `"[ipv6]:port"`, `"[fe80::1%3]:port"`, or a URL
(`"http://host/"`, default ports `http` 80, `https` 443, `ftp` 21, `ssh` 22,
`telnet` 23). Servers: `"0.0.0.0:port"` (IPv4), `"[::]:port"` (IPv6 and
IPv4), `"127.0.0.1:port"` (local only).

### Conversions

| Expression | Result |
|------------|--------|
| `typeof(s)` | `"Socket"` |
| `s instanceof Socket` | `true` |
| `"" + s` | `"Socket"` |
| `Convert.toBoolean(s)`, `if (s)` | `true`, even when closed |
| `s.x = 1` | throws |

### Errors

| Message | Cause |
|---------|-------|
| `invalid parameter` | a method called with a `this` that is not a socket, or `readToBuffer` / `writeFromBuffer` with a non `Buffer` argument |
| `setPropertyBySymbol` | assigning a property on a socket |
| `Unable to open "Socket"` | the extension library was not found and no internal one is registered |

Network failures never throw: they are reported by the return values
(`false`, `undefined`, `0`, `-1`).

## C++

Namespace `XYO::QuantumScript::Extension::Socket`, umbrella header
`<XYO/QuantumScript.Extension/Socket.hpp>`.

### Library (`Socket/Library.hpp`)

| Symbol | Notes |
|--------|-------|
| `void registerInternalExtension(Executive *executive)` | register `"Socket"` as an internal extension |
| `void initExecutive(Executive *executive, void *extensionId)` | extension init, run by the engine |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | DLL entry point (not in static builds) |

### `VariableSocket` (`Socket/VariableSocket.hpp`)

| Symbol | Notes |
|--------|-------|
| `XYO::Networking::Socket value` | the connection |
| `static Variable *newVariable()` | new, closed socket |
| `void activeDestructor()` | `value.close()` |
| `String getVariableType()` | `"Socket"` |
| `Variable *instancePrototype()` | `Socket.prototype` of the current thread |
| `bool toBoolean()` | `true` |
| `String toString()` | `"Socket"` |

### `SocketContext` (`Socket/Context.hpp`)

| Symbol | Notes |
|--------|-------|
| `Symbol symbolFunctionSocket` | symbol of the `Socket` global |
| `TPointerX<Prototype> prototypeSocket` | `Socket.prototype` holder |
| `SocketContext *getContext()` | per thread singleton |

### Metadata

| Symbol | Notes |
|--------|-------|
| `Version::version()`, `Version::build()`, `Version::versionWithBuild()`, `Version::datetime()` | from `version.json` |
| `Copyright::copyright()`, `Copyright::publisher()`, `Copyright::company()`, `Copyright::contact()` | |
| `License::license()`, `License::shortLicense()` | MIT text |

`Version`, `Copyright` and `License` exist in every XYO library: qualify them
(`Extension::Socket::Version::versionWithBuild()`).

### Build configuration

| Name | Meaning |
|------|---------|
| `quantum-script--socket` | fabricare project, `dll-or-lib`: DLL / shared library, static library on static builds |
| `XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT` | export / import macro |
| `XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_INTERNAL` | defined while building the DLL (from `QUANTUM_SCRIPT__SOCKET_INTERNAL`) |
| `XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_LIBRARY` | empty export macro, no DLL entry point |
