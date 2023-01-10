# Quantum Script Extension Socket

```javascript
Socket();
Socket.isSocket(x);
Socket.prototype.openServer(addr);
Socket.prototype.openClient(addr);
Socket.prototype.read(size);
Socket.prototype.readLn(size);
Socket.prototype.write(str);
Socket.prototype.writeLn(str);
Socket.prototype.listen(count);
Socket.prototype.close();
Socket.prototype.accept();
Socket.prototype.waitToWrite(microSec);
Socket.prototype.waitToRead(microSec);
Socket.prototype.readToBuffer(buffer);
Socket.prototype.writeFromBuffer(buffer);
Socket.prototype.becomeOwner(socket);
Socket.prototype.linkOwner(socket);
Socket.prototype.unLinkOwner(socket);
Socket.prototype.transferOwner(socket);
```

## License

Copyright (c) 2016-2023 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
