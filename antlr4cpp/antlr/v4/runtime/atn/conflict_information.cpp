/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
#include "stdafx.h"

#include <antlr/v4/runtime/atn/conflict_information.hpp>

namespace antlr4 {
namespace runtime {
namespace atn {

	bool operator== (conflict_information const& /*x*/, conflict_information const& /*y*/)
	{
		throw std::runtime_error("not implemented");
	}

}
}
}

namespace std {

	using namespace antlr4::runtime::atn;

	inline bool hash<conflict_information>::operator()(conflict_information const& /*x*/) const
	{
		throw std::runtime_error("not implemented");
	}

}
