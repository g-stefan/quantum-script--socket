//
// Quantum Script Extension Socket
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_VARIABLESOCKET_HPP
#define QUANTUM_SCRIPT_VARIABLESOCKET_HPP

#ifndef QUANTUM_SCRIPT_VARIABLE_HPP
#include "quantum-script-variable.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Socket {

				class VariableSocket;

			};
		};

	};
};


namespace XYO {
	namespace ManagedMemory {
		template<>
		class TMemory<Quantum::Script::Extension::Socket::VariableSocket>:
			public TMemoryPoolActive<Quantum::Script::Extension::Socket::VariableSocket> {};
	};
};


namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Socket {

				using namespace XYO;

				class VariableSocket :
					public Variable {
						XYO_DISALLOW_COPY_ASSIGN_MOVE(VariableSocket);
						XYO_DYNAMIC_TYPE_DEFINE(QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT, VariableSocket);
					protected:
						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT static const char *strTypeSocket;
					public:

						XYO::Socket value;

						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT VariableSocket();

						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT void activeDestructor();

						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT static Variable *newVariable();

						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT String getVariableType();

						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT Variable *instancePrototype();

						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT Variable *clone(SymbolList &inSymbolList);

						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT bool toBoolean();
						QUANTUM_SCRIPT_EXTENSION_SOCKET_EXPORT String toString();

				};


			};
		};
	};
};


#endif

