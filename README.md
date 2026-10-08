# Quantum Script Extension Socket

Quantum Script extension
- A `Socket` type for TCP networking in scripts, IPv4 and IPv6, on Windows
and Linux.
- Clients (`openClient("host:port")`) and servers (`openServer`, `listen`,
`accept`), addresses given as text (`"127.0.0.1:80"`, `"[::1]:80"`,
`"example.com:443"`, `"http://example.com/"`).
- Text (`write`, `writeLn`, `read`, `readLn`) and binary data through
`Buffer` (`readToBuffer`, `writeFromBuffer`).
- Timeouts with `waitToRead` / `waitToWrite`.

```javascript
Script.requireExtension("Socket");

Socket();
Socket.isSocket(x);
Socket.prototype.openServer(addr);
Socket.prototype.openClient(addr);
Socket.prototype.read(size);
Socket.prototype.readLn(size);
Socket.prototype.write(str);
Socket.prototype.writeLn(str);
Socket.prototype.listen(count);
Socket.prototype.shutdown();
Socket.prototype.close();
Socket.prototype.accept();
Socket.prototype.waitToWrite(microSec);
Socket.prototype.waitToRead(microSec);
Socket.prototype.readToBuffer(buffer,ln);
Socket.prototype.writeFromBuffer(buffer);
```

Built on `quantum-script` and `xyo-networking`, part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, a first client and server, register in a C++ host, static builds, threads
- [Script API](docs/script-api.md) - every function: exact behavior, address text, edge cases
- [Recipes](docs/recipes.md) - HTTP GET, exact reads, framed messages, timeouts, a server that can stop, file transfer, loopback tests
- [C++ API](docs/cpp-api.md) - `VariableSocket`, using sockets in native functions
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--socket](.claude/skills/quantum-script--socket/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
