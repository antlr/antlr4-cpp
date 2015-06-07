// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <cassert>
#include <stdexcept>

#include "atn_state.hpp"
#include "transition.hpp"

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) )
#endif

namespace antlr4 {
namespace atn {

	void atn_state::add_transition(std::shared_ptr<atn::transition> const& transition)
	{
		add_transition(_transitions.size(), transition);
	}

	void atn_state::add_transition(size_t index, std::shared_ptr<atn::transition> const& transition)
	{
		if (_transitions.empty())
		{
			_epsilon_only_transitions = transition->epsilon();
		}
		else if (_epsilon_only_transitions != transition->epsilon())
		{
			assert(!"ATN state cannot have both epsilon and non-epsilon transitions.");
			_epsilon_only_transitions = false;
		}

		_transitions.insert(_transitions.begin() + static_cast<ptrdiff_t>(index), transition);
	}

	void atn_state::set_transition(size_t index, std::shared_ptr<atn::transition> const& transition)
	{
		_transitions[index] = transition;
	}

	std::shared_ptr<atn::transition> atn_state::remove_transition(size_t index)
	{
		std::shared_ptr<atn::transition> result(std::move(_transitions.at(index)));
		_transitions.erase(_transitions.begin() + static_cast<ptrdiff_t>(index));
		return std::move(result);
	}

	void atn_state::add_optimized_transition(size_t index, std::shared_ptr<atn::transition> const& transition)
	{
		_optimized = true;
		_optimized_transitions.insert(_optimized_transitions.begin() + static_cast<ptrdiff_t>(index), transition);
	}

	void atn_state::set_optimized_transition(size_t index, std::shared_ptr<atn::transition> const& transition)
	{
		assert(optimized());
		_optimized_transitions[index] = transition;
	}

	std::shared_ptr<atn::transition> atn_state::remove_optimized_transition(size_t index)
	{
		assert(optimized());
		std::shared_ptr<atn::transition> result(std::move(_optimized_transitions.at(index)));
		_optimized_transitions.erase(_optimized_transitions.begin() + static_cast<ptrdiff_t>(index));
		return std::move(result);
	}

	std::shared_ptr<star_loop_entry_state> star_loopback_state::loop_entry_state() const
	{
		return std::static_pointer_cast<star_loop_entry_state>(transition(0)->target());
	}

}
}
