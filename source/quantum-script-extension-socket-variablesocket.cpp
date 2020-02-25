//
// Quantum Script Extension Socket
//
// Copyright (c) 2020 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "quantum-script-context.hpp"
#include "quantum-script-variablenumber.hpp"
#include "quantum-script-variablestring.hpp"

#include "quantum-script-extension-socket.hpp"
#include "quantum-script-extension-socket-variablesocket.hpp"


namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Socket {

				using namespace XYO;

				const char *VariableSocket::typeSocketKey = "{DBB0BD0F-547B-445D-B070-0A744845F8F4}";
				const void *VariableSocket::typeSocket;
				const char *VariableSocket::strTypeSocket = "Socket";

				String VariableSocket::getType() {
					return strTypeSocket;
				};

				Variable *VariableSocket::newVariable() {
					return (Variable *) TMemory<VariableSocket>::newMemory();
				};

				Variable &VariableSocket::operatorReference(Symbol symbolId) {
					return operatorReferenceX(symbolId, (Extension::Socket::getContext())->prototypeSocket->prototype);
				};

				Variable *VariableSocket::instancePrototype() {
					return (Extension::Socket::getContext())->prototypeSocket->prototype;
				};

				void VariableSocket::activeDestructor() {
					value.close();
				};

				Variable *VariableSocket::clone(SymbolList &inSymbolList) {
					VariableSocket *out = (VariableSocket *)newVariable();
					out->value.becomeOwner(value);
					return  out;
				};

				bool VariableSocket::toBoolean() {
					return true;
				};

				String VariableSocket::toString() {
					return strTypeSocket;
				};

			};
		};
	};
};


