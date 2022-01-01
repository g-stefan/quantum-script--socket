//
// Quantum Script Extension Socket
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_SOCKET_HPP
#define QUANTUM_SCRIPT_EXTENSION_SOCKET_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SOCKET__EXPORT_HPP
#include "quantum-script-extension-socket--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SOCKET_COPYRIGHT_HPP
#include "quantum-script-extension-socket-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SOCKET_LICENSE_HPP
#include "quantum-script-extension-socket-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SOCKET_VERSION_HPP
#include "quantum-script-extension-socket-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Socket {

				using namespace Quantum::Script;

				class SocketContext:
					public Object {
						XYO_DISALLOW_COPY_ASSIGN_MOVE(SocketContext);
					public:

						Symbol symbolFunctionSocket;
						TPointerX<Prototype> prototypeSocket;

						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT SocketContext();
				};

				QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT SocketContext *getContext();

				QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif

