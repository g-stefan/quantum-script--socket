# C++ API

For hosts that embed Quantum Script and for extensions that take or return
sockets. Read the `quantum-script` repository's `docs/embedding.md` and
`docs/writing-extensions.md` first: native functions, `Variable` and
`TPointer` work the same way here. The socket itself is
`XYO::Networking::Socket`, documented in the `xyo-networking` repository,
`docs/sockets.md`.

## Headers and namespace

```cpp
#include <XYO/QuantumScript.Extension/Socket.hpp>                   // Library.hpp
#include <XYO/QuantumScript.Extension/Socket/VariableSocket.hpp>    // the value type
#include <XYO/QuantumScript.Extension/Socket/Context.hpp>           // SocketContext (rarely needed)

using namespace XYO::QuantumScript;
using Extension::Socket::VariableSocket;
```

Namespace: `XYO::QuantumScript::Extension::Socket`. Export macro:
`XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT` (empty when
`XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_LIBRARY` is defined, i.e. static
builds). `Dependency.hpp` includes `<XYO/QuantumScript.hpp>` and
`<XYO/Networking.hpp>`.

## Registering the extension

```cpp
void Extension::Socket::registerInternalExtension(Executive *executive);
void Extension::Socket::initExecutive(Executive *executive, void *extensionId);
```

- `registerInternalExtension` registers `"Socket"` as an internal
  extension; call it from the host's init callback, after
  `Extension::Buffer::registerInternalExtension` (see
  [Getting started](getting-started.md#5-register-it-in-a-c-host)).
- `initExecutive` is the extension's init function, run by the engine when a
  script first requires `Socket` in a thread. It sets the extension name,
  info (license text) and version, creates the `Socket` global function and
  its prototype, runs `Script.requireExtension("Buffer")`, and registers the
  native methods. Do not call it directly.
- The DLL build also exports
  `extern "C" void quantumScriptExtension(Executive *, void *)`, which
  forwards to `initExecutive`; it is what `Script.requireExtension` looks up
  in `quantum-script--socket.dll`.

## `VariableSocket`

```cpp
class VariableSocket : public Variable {
	public:
		XYO::Networking::Socket value;    // the connection

		static Variable *newVariable();   // a new, closed socket

		void activeDestructor();          // value.close(), when the object is recycled

		// Variable overrides
		String getVariableType();         // "Socket"
		Variable *instancePrototype();    // Socket.prototype of the current thread
		bool toBoolean();                 // always true
		String toString();                // "Socket"
};
```

`value` is a public `XYO::Networking::Socket`: everything the script API
does not expose (`getIPAddress4` / `getIPAddress6`, `openClient(IPAddress4 &)`,
raw `read(void *, size_t)`) is available to C++ code through it.

`VariableSocket` does not override `clone`: a socket cannot be copied to
another thread.

## Taking a socket argument

Check the type with `TIsType<VariableSocket>`, then use `value`:

```cpp
static TPointer<Variable> peerAddress(VariableFunction *function, Variable *this_, VariableArray *arguments) {
	TPointerX<Variable> &socketV(arguments->index(0));
	if (!TIsType<VariableSocket>(socketV)) {
		throw(Error("invalid parameter"));
	};
	XYO::Networking::Socket &socket = ((VariableSocket *)socketV.value())->value;
	XYO::Networking::IPAddress4 address;
	if (!socket.getIPAddress4(address)) {
		return Context::getValueUndefined();
	};
	char text[32];
	snprintf(text, sizeof(text), "%u.%u.%u.%u:%u", address.ip.ip[0], address.ip.ip[1],
	         address.ip.ip[2], address.ip.ip[3], address.port);
	return VariableString::newVariable(text);
};
```

For text and buffers, the extension itself uses the `xyo-system` stream
helpers on `value` (it is an `IRead` / `IWrite`):
`Stream::read(socket, out, size)`, `Stream::readLn(socket, out, size)`,
`Stream::readToBuffer(socket, buffer, ln)`.

## Returning a socket

```cpp
TPointer<Variable> retV(VariableSocket::newVariable());
if (((VariableSocket *)retV.value())->value.openClientX("127.0.0.1:8080")) {
	return retV;
};
return Context::getValueUndefined();
```

This is the `accept()` pattern: create the variable, open its `value` in
place (`server.accept(((VariableSocket *)retV.value())->value)`), return it
on success. The script must have loaded `Socket` in that thread so
`Socket.prototype` exists.

## Depending on Socket from another extension

1. In `fabricare.json`, add `"quantum-script--socket"` to the project's
   dependencies.
2. Include `<XYO/QuantumScript.Extension/Socket/VariableSocket.hpp>`.
3. In your `initExecutive`, load the extension before anything that may
   create or receive sockets:

```cpp
void initExecutive(Executive *executive, void *extensionId) {
	executive->setExtensionName(extensionId, "Remote");
	// ...
	executive->compileStringX("Script.requireExtension(\"Socket\");");
	executive->compileStringX("var Remote={};");
	executive->setFunction2("Remote.peerAddress(socket)", peerAddress);
};
```

An extension written in script (as `quantum-script--http` is) only needs
`Script.requireExtension("Socket");` at the top.

## `SocketContext`

```cpp
class SocketContext : public Object {
	public:
		Symbol symbolFunctionSocket;            // the "Socket" global
		TPointerX<Prototype> prototypeSocket;   // Socket.prototype
};
SocketContext *getContext();                    // TSingleton<SocketContext>: one per thread
```

Filled by `initExecutive`, cleared by the extension's delete-context hook
when the engine ends.

## Notes for maintainers

- Every native method checks `TIsType<VariableSocket>(this_)` and throws
  `Error("invalid parameter")` otherwise.
- Native methods live in `Socket/Library.cpp` as
  `static TPointer<Variable> name(VariableFunction *, Variable *this_, VariableArray *arguments)`
  and are registered in `initExecutive` with
  `executive->setFunction2("Socket.prototype.name(args)", name)`. The
  argument names in that string are what scripts see in error messages and
  must match the arguments the function reads (`readToBuffer(buffer,ln)`).
- New methods: update `README.md`, `docs/script-api.md`,
  `docs/reference.md` and the skill in `.claude/skills/quantum-script--socket/`.
- There is no test project yet; the loopback pattern of
  [Recipes](recipes.md#client-and-server-in-one-script) (client and server
  in one script) is a good base for one.
