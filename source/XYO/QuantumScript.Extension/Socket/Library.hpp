// Quantum Script Extension Socket
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_LIBRARY_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_LIBRARY_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Socket/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Socket {

	XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT void initExecutive(Executive *executive, void *extensionId);
	XYO_QUANTUMSCRIPT_EXTENSION_SOCKET_EXPORT void registerInternalExtension(Executive *executive);

};

#endif
