// Quantum Script Extension Socket
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Socket/License.hpp>

namespace XYO::QuantumScript::Extension::Socket::License {

	const char *license() {
		return XYO::ManagedMemory::License::license();
	};

	const char *shortLicense() {
		return XYO::ManagedMemory::License::shortLicense();
	};

};
