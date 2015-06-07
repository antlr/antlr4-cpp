// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>
#include "../misc/unordered_ptr_map.hpp"

namespace antlr4 {

	namespace atn {
		class atn_state;
	}

namespace dfa {

	class dfa_state;

	class dynamic_dfa
	{
	private:
		misc::unordered_ptr_map<std::shared_ptr<dfa_state>, std::shared_ptr<dfa_state>> _states;
		std::shared_ptr<dfa_state> _s0;
		std::shared_ptr<dfa_state> _s0_full;

		const size_t _decision;
		const std::shared_ptr<atn::atn_state> _start_state;
		const int32_t _min_dfa_edge;
		const int32_t _max_dfa_edge;

		size_t _next_state_number;

		bool _precedence_dfa;

	public:
		dynamic_dfa(std::shared_ptr<atn::atn_state> const& start_state)
			: dynamic_dfa(start_state, 0)
		{
		}

		dynamic_dfa(std::shared_ptr<atn::atn_state> const& start_state, size_t decision);

	public:
		int32_t min_dfa_edge() const
		{
			return _min_dfa_edge;
		}

		int32_t max_dfa_edge() const
		{
			return _max_dfa_edge;
		}

		bool precedence_dfa() const
		{
			return _precedence_dfa;
		}

		void precedence_dfa(bool value);

		std::shared_ptr<dfa_state> const& precedence_start_state(int32_t precedence, bool full_context) const;

		void precedence_start_state(int32_t precedence, bool full_context, std::shared_ptr<dfa_state> const& start_state);

		bool empty() const;

		bool context_sensitive() const;

		std::shared_ptr<dfa_state> add_state(std::shared_ptr<dfa_state> const& state);
	};

}
}
