// Quantum Script Extension Socket
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Socket/Library.hpp>
#include <XYO/QuantumScript.Extension/Socket/Copyright.hpp>
#include <XYO/QuantumScript.Extension/Socket/License.hpp>
#include <XYO/QuantumScript.Extension/Socket/Version.hpp>
#include <XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>
#include <XYO/QuantumScript.Extension/Socket/Context.hpp>
#include <XYO/QuantumScript.Extension/Socket/VariableSocket.hpp>

namespace XYO::QuantumScript::Extension::Socket {
	using namespace XYO::QuantumScript::Extension::Buffer;

	SocketContext::SocketContext() {
		symbolFunctionSocket = 0;
		prototypeSocket.pointerLink(this);
	};

	SocketContext *getContext() {
		return TSingleton<SocketContext>::getValue();
	};

	static TPointer<Variable> functionSocket(VariableFunction *function, Variable *this_, VariableArray *arguments) {
		return VariableSocket::newVariable();
	};

	static void deleteContext() {
		SocketContext *socketContext = getContext();
		socketContext->prototypeSocket.deleteMemory();
		socketContext->symbolFunctionSocket = 0;
	};

	static void newContext(Executive *executive, void *extensionId) {
		VariableFunction *defaultPrototypeFunction;

		SocketContext *socketContext = getContext();
		executive->setExtensionDeleteContext(extensionId, deleteContext);

		socketContext->symbolFunctionSocket = Context::getSymbol("Socket");
		socketContext->prototypeSocket.newMemory();

		defaultPrototypeFunction = (VariableFunction *)VariableFunction::newVariable(nullptr, nullptr, nullptr, functionSocket, nullptr, nullptr);
		(Context::getGlobalObject())->setPropertyBySymbol(socketContext->symbolFunctionSocket, defaultPrototypeFunction);
		socketContext->prototypeSocket = defaultPrototypeFunction->prototype;
	};

	static TPointer<Variable> isSocket(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- script-is-socket\n");
#endif
		return VariableBoolean::newVariable(TIsType<VariableSocket>(arguments->index(0)));
	};

	static TPointer<Variable> openServer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-open-server\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		return VariableBoolean::newVariable(((VariableSocket *)(this_))->value.openServerX((arguments->index(0))->toString()));
	};

	static TPointer<Variable> openClient(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-open-client\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		return VariableBoolean::newVariable(((VariableSocket *)(this_))->value.openClientX((arguments->index(0))->toString()));
	};

	static TPointer<Variable> socketRead(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-read\n");
#endif
		String retV;
		Number ln;

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		if (TIsTypeExact<VariableUndefined>(arguments->index(0))) {
			ln = 32768;
		} else {
			ln = (arguments->index(0))->toNumber();
			if (isnan(ln) || isinf(ln) || signbit(ln)) {
				return Context::getValueUndefined();
			};
		};

		if (Stream::read(((VariableSocket *)this_)->value, retV, ln)) {
			return VariableString::newVariable(retV);
		};

		return Context::getValueUndefined();
	};

	static TPointer<Variable> socketReadLn(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-read-ln\n");
#endif
		String retV;
		Number ln;

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		if (TIsTypeExact<VariableUndefined>(arguments->index(0))) {
			ln = 32768;
		} else {

			ln = (arguments->index(0))->toNumber();
			if (isnan(ln) || isinf(ln) || signbit(ln)) {
				return Context::getValueUndefined();
			};
		};

		if (Stream::readLn(((VariableSocket *)this_)->value, retV, ln)) {
			return VariableString::newVariable(retV);
		};

		return Context::getValueUndefined();
	};

	static TPointer<Variable> socketWrite(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-write\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		String toWrite = (arguments->index(0))->toString();
		return VariableNumber::newVariable((Number)(((VariableSocket *)this_)->value.write(toWrite.value(), toWrite.length())));
	};

	static TPointer<Variable> socketWriteLn(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-write-ln\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		String toWrite = (arguments->index(0))->toString();
		toWrite << "\r\n";
		return VariableNumber::newVariable((Number)(((VariableSocket *)this_)->value.write(toWrite.value(), toWrite.length())));
	};

	static TPointer<Variable> socketListen(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-listen\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		Number count_ = (arguments->index(0))->toNumber();
		if (isnan(count_) || isinf(count_) || signbit(count_)) {
			return VariableBoolean::newVariable(false);
		};

		return VariableBoolean::newVariable(((VariableSocket *)this_)->value.listen((uint16_t)count_));
	};

	static TPointer<Variable> socketShutdown(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-shutdown\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		((VariableSocket *)this_)->value.shutdown();

		return Context::getValueUndefined();
	};

	static TPointer<Variable> socketClose(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-close\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		((VariableSocket *)this_)->value.close();

		return Context::getValueUndefined();
	};

	static TPointer<Variable> socketAccept(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-accept\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		VariableSocket *socket_ = (VariableSocket *)VariableSocket::newVariable();

		if (((VariableSocket *)this_)->value.accept(socket_->value)) {
			return socket_;
		};
		socket_->decReferenceCount();
		return Context::getValueUndefined();
	};

	static TPointer<Variable> waitToWrite(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-wait-to-write\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		Number microSeconds = (arguments->index(0))->toNumber();
		if (isnan(microSeconds) || isinf(microSeconds) || signbit(microSeconds)) {
			return VariableNumber::newVariable(NAN);
		};

		return VariableNumber::newVariable(((VariableSocket *)this_)->value.waitToWrite((uint32_t)microSeconds));
	};

	static TPointer<Variable> waitToRead(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-wait-to-read\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		Number microSeconds = (arguments->index(0))->toNumber();
		if (isnan(microSeconds) || isinf(microSeconds) || signbit(microSeconds)) {
			return VariableNumber::newVariable(NAN);
		};

		return VariableNumber::newVariable(((VariableSocket *)this_)->value.waitToRead((uint32_t)microSeconds));
	};

	static TPointer<Variable> socketReadToBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-read-to-buffer\n");
#endif
		Number ln;

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		TPointerX<Variable> &buffer(arguments->index(0));

		if (!TIsType<Extension::Buffer::VariableBuffer>(buffer)) {
			throw(Error("invalid parameter"));
		};

		if (TIsTypeExact<VariableUndefined>(arguments->index(0))) {
			ln = 32768;
		} else {
			ln = (arguments->index(1))->toNumber();
			if (isnan(ln) || signbit(ln) || ln == 0.0) {
				((Extension::Buffer::VariableBuffer *)buffer.value())->buffer.length = 0;
				return VariableNumber::newVariable(0);
			};
			if (isinf(ln)) {
				ln = ((Extension::Buffer::VariableBuffer *)buffer.value())->buffer.size;
			};
		};

		if (ln > ((Extension::Buffer::VariableBuffer *)buffer.value())->buffer.size) {
			ln = ((Extension::Buffer::VariableBuffer *)buffer.value())->buffer.size;
		};

		return VariableNumber::newVariable(Stream::readToBuffer(((VariableSocket *)this_)->value, ((Extension::Buffer::VariableBuffer *)buffer.value())->buffer, ln));
	};

	static TPointer<Variable> socketWriteFromBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- socket-write-from-buffer\n");
#endif

		if (!TIsType<VariableSocket>(this_)) {
			throw(Error("invalid parameter"));
		};

		TPointerX<Variable> &buffer(arguments->index(0));

		if (!TIsType<Extension::Buffer::VariableBuffer>(buffer)) {
			throw(Error("invalid parameter"));
		};

		return VariableNumber::newVariable((Number)(((VariableSocket *)this_)->value.write(((Extension::Buffer::VariableBuffer *)buffer.value())->buffer.buffer, ((Extension::Buffer::VariableBuffer *)buffer.value())->buffer.length)));
	};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("Socket", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {

		String info = "Socket\r\n";
		info << License::shortLicense().c_str();

		executive->setExtensionName(extensionId, "Socket");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::Socket::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		newContext(executive, extensionId);

		executive->compileStringX("Script.requireExtension(\"Buffer\");");
		executive->setFunction2("Socket.isSocket(x)", isSocket);
		executive->setFunction2("Socket.prototype.openServer(addr)", openServer);
		executive->setFunction2("Socket.prototype.openClient(addr)", openClient);
		executive->setFunction2("Socket.prototype.read(size)", socketRead);
		executive->setFunction2("Socket.prototype.readLn(size)", socketReadLn);
		executive->setFunction2("Socket.prototype.write(str)", socketWrite);
		executive->setFunction2("Socket.prototype.writeLn(str)", socketWriteLn);
		executive->setFunction2("Socket.prototype.listen(count)", socketListen);
		executive->setFunction2("Socket.prototype.shutdown()", socketShutdown);
		executive->setFunction2("Socket.prototype.close()", socketClose);
		executive->setFunction2("Socket.prototype.accept()", socketAccept);
		executive->setFunction2("Socket.prototype.waitToWrite(microSec)", waitToWrite);
		executive->setFunction2("Socket.prototype.waitToRead(microSec)", waitToRead);
		executive->setFunction2("Socket.prototype.readToBuffer(buffer)", socketReadToBuffer);
		executive->setFunction2("Socket.prototype.writeFromBuffer(buffer)", socketWriteFromBuffer);
	};

};

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_LIBRARY
#	ifdef XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::Socket::initExecutive(executive, extensionId);
};
#	endif
#endif
