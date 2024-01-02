// Quantum Script Extension Socket
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_CONTEXT_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_CONTEXT_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Socket/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Socket {

	class SocketContext : public Object {
			XYO_DISALLOW_COPY_ASSIGN_MOVE(SocketContext);

		public:
			Symbol symbolFunctionSocket;
			TPointerX<Prototype> prototypeSocket;

			XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT SocketContext();
	};

	XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT SocketContext *getContext();
};

#endif
