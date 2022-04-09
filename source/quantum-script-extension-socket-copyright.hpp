//
// Quantum Script Extension Socket
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_SOCKET_COPYRIGHT_HPP
#define QUANTUM_SCRIPT_EXTENSION_SOCKET_COPYRIGHT_HPP

#define QUANTUM_SCRIPT_EXTENSION_SOCKET_COPYRIGHT "Copyright (c) Grigore Stefan"
#define QUANTUM_SCRIPT_EXTENSION_SOCKET_PUBLISHER "Grigore Stefan"
#define QUANTUM_SCRIPT_EXTENSION_SOCKET_COMPANY QUANTUM_SCRIPT_EXTENSION_SOCKET_PUBLISHER
#define QUANTUM_SCRIPT_EXTENSION_SOCKET_CONTACT "g_stefan@yahoo.com"
#define QUANTUM_SCRIPT_EXTENSION_SOCKET_FULL_COPYRIGHT QUANTUM_SCRIPT_EXTENSION_SOCKET_COPYRIGHT " <" QUANTUM_SCRIPT_EXTENSION_SOCKET_CONTACT ">"

#ifndef XYO_RC

#	ifndef QUANTUM_SCRIPT_EXTENSION_SOCKET__EXPORT_HPP
#		include "quantum-script-extension-socket--export.hpp"
#	endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Socket {
				namespace Copyright {
					QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT const char *copyright();
					QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT const char *publisher();
					QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT const char *company();
					QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT const char *contact();
					QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT const char *fullCopyright();
				};
			};
		};
	};
};

#endif
#endif
