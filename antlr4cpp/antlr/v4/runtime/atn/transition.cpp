// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <cassert>
#include <stdexcept>

#include "transition.hpp"

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) )
#endif

namespace antlr4 {
namespace atn {

	bool transition::matches(int32_t symbol, int32_t min_vocab, int32_t max_vocab) const
	{
		switch (type())
		{
		case transition_type::epsilon:
		case transition_type::rule:
		case transition_type::predicate:
		case transition_type::action:
		case transition_type::precedence:
			return false;

		case transition_type::range:
		{
			range_transition const* transition = static_cast<range_transition const*>(this);
			std::pair<int32_t, int32_t> label = transition->label();
			return symbol >= label.first && symbol < label.second;
		}

		case transition_type::atom:
		{
			atom_transition const* transition = static_cast<atom_transition const*>(this);
			return transition->label() == symbol;
		}

		case transition_type::set:
		{
			//set_transition const* transition = static_cast<set_transition const*>(this);
			throw std::runtime_error("not implemented");
		}

		case transition_type::not_set:
		{
			//not_set_transition const* transition = static_cast<not_set_transition const*>(this);
			throw std::runtime_error("not implemented");
		}

		case transition_type::wildcard:
			return symbol >= min_vocab && symbol <= max_vocab;

		default:
			assert(!"Invalid transition type.");
			return false;
		}
	}

}
}
