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
