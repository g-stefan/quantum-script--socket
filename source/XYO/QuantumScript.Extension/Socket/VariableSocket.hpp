// Quantum Script Extension Socket
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_VARIABLEFILE_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_VARIABLEFILE_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Socket/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Socket {

	class VariableFile;
};

namespace XYO::ManagedMemory {
	template <>
	class TMemory<XYO::QuantumScript::Extension::Socket::VariableFile> : public TMemoryPoolActive<XYO::QuantumScript::Extension::Socket::VariableFile> {};
};

namespace XYO::QuantumScript::Extension::Socket {

	using namespace XYO;

	class VariableSocket : public Variable {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(VariableSocket);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT, VariableSocket);

		protected:
			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT static const char *strTypeSocket;

		public:
			XYO::Networking::Socket value;

			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT VariableSocket();

			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT void activeDestructor();

			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT static Variable *newVariable();

			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT String getVariableType();

			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT Variable *instancePrototype();

			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT bool toBoolean();
			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT String toString();
	};
};

#endif
