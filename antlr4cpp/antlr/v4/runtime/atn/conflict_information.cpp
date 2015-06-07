// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <antlr/v4/runtime/atn/conflict_information.hpp>

namespace antlr4 {
namespace atn {

	bool operator== (conflict_information const& /*x*/, conflict_information const& /*y*/)
	{
		throw std::runtime_error("not implemented");
	}

}
}

namespace std {

	using namespace antlr4::atn;

	inline bool hash<conflict_information>::operator()(conflict_information const& /*x*/) const
	{
		throw std::runtime_error("not implemented");
	}

}
