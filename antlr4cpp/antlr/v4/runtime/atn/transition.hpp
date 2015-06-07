// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>
#include "semantic_context.hpp"

namespace antlr4 {
namespace atn {

	class atn_state;
	class rule_start_state;

	class transition {
	public:
		enum class transition_type
		{
			epsilon = 1,
			range,
			rule,
			predicate,
			atom,
			action,
			set,
			not_set,
			wildcard,
			precedence,
		};

	private:
		const transition_type _type;
		const std::shared_ptr<atn_state> _target;

	public:
		transition(transition_type type, std::shared_ptr<atn_state> target)
			: _type(type)
			, _target(target)
		{
		}

	public:
		transition_type type() const
		{
			return _type;
		}

		std::shared_ptr<atn_state> const& target() const
		{
			return _target;
		}

		bool epsilon() const
		{
			switch (type())
			{
			case transition_type::epsilon:
			case transition_type::rule:
			case transition_type::predicate:
			case transition_type::action:
			case transition_type::precedence:
				return true;

			case transition_type::range:
			case transition_type::atom:
			case transition_type::set:
			case transition_type::not_set:
			case transition_type::wildcard:
			default:
				return false;
			}
		}

		bool matches(int32_t symbol, int32_t min_vocab, int32_t max_vocab) const;
	};

	class epsilon_transition : public transition
	{
	private:
		const int32_t _outermost_precedence_return;

	public:
		explicit epsilon_transition(std::shared_ptr<atn_state> const& target, int32_t outermost_precedence_return)
			: transition(transition_type::epsilon, target)
			, _outermost_precedence_return(outermost_precedence_return)
		{
		}

	public:
		int32_t outermost_precedence_return() const
		{
			return _outermost_precedence_return;
		}
	};

	class range_transition : public transition
	{
	private:
		const std::pair<int32_t, int32_t> _label;

	public:
		explicit range_transition(std::shared_ptr<atn_state> const& target, std::pair<int32_t, int32_t> label)
			: transition(transition_type::range, target)
			, _label(label)
		{
		}

	public:
		std::pair<int32_t, int32_t> label() const
		{
			return _label;
		}
	};

	class rule_transition : public transition
	{
	private:
		const size_t _rule_index;
		const int32_t _precedence;
		const std::shared_ptr<atn_state> _follow_state;

		bool _tail_call;
		bool _optimized_tail_call;

	public:
		explicit rule_transition(std::shared_ptr<atn_state> const& target, size_t rule_index, int32_t precedence, std::shared_ptr<atn_state> const& follow_state)
			: transition(transition_type::rule, target)
			, _rule_index(rule_index)
			, _precedence(precedence)
			, _follow_state(follow_state)
		{
		}

	public:
		size_t rule_index() const
		{
			return _rule_index;
		}

		int32_t precedence() const
		{
			return _precedence;
		}

		std::shared_ptr<atn_state> const& follow_state() const
		{
			return _follow_state;
		}

		bool tail_call() const
		{
			return _tail_call;
		}

		bool optimized_tail_call() const
		{
			return _optimized_tail_call;
		}

	public:
		void tail_call(bool value)
		{
			_tail_call = value;
		}

		void optimized_tail_call(bool value)
		{
			_optimized_tail_call = value;
		}
	};

	class predicate_transition : public transition
	{
	private:
		const size_t _rule_index;
		const size_t _predicate_index;
		const bool _context_dependent;

	public:
		explicit predicate_transition(std::shared_ptr<atn_state> const& target, size_t rule_index, size_t predicate_index, bool context_dependent)
			: transition(transition_type::predicate, target)
			, _rule_index(rule_index)
			, _predicate_index(predicate_index)
			, _context_dependent(context_dependent)
		{
		}

	public:
		size_t rule_index() const
		{
			return _rule_index;
		}

		size_t predicate_index() const
		{
			return _predicate_index;
		}

		bool context_dependent() const
		{
			return _context_dependent;
		}
	};

	class atom_transition : public transition
	{
	private:
		const int32_t _label;

	public:
		explicit atom_transition(std::shared_ptr<atn_state> const& target, int32_t label)
			: transition(transition_type::atom, target)
			, _label(label)
		{
		}

	public:
		int32_t label() const
		{
			return _label;
		}
	};

	class action_transition : public transition
	{
	private:
		const size_t _rule_index;
		const size_t _action_index;
		const bool _context_dependent;

	public:
		explicit action_transition(std::shared_ptr<atn_state> const& target, size_t rule_index, size_t action_index, bool context_dependent)
			: transition(transition_type::action, target)
			, _rule_index(rule_index)
			, _action_index(action_index)
			, _context_dependent(context_dependent)
		{
		}

	public:
		size_t rule_index() const
		{
			return _rule_index;
		}

		size_t action_index() const
		{
			return _action_index;
		}

		bool context_dependent() const
		{
			return _context_dependent;
		}
	};

	class set_transition : public transition
	{
	public:
		explicit set_transition(std::shared_ptr<atn_state> const& target)
			: set_transition(transition_type::epsilon, target)
		{
		}

	protected:
		explicit set_transition(transition_type type, std::shared_ptr<atn_state> const& target)
			: transition(type, target)
		{
			throw std::runtime_error("not implemented");
		}
	};

	class not_set_transition : public set_transition
	{
	public:
		explicit not_set_transition(std::shared_ptr<atn_state> const& target)
			: set_transition(transition_type::not_set, target)
		{
			throw std::runtime_error("not implemented");
		}
	};

	class wildcard_transition : public transition
	{
	public:
		explicit wildcard_transition(std::shared_ptr<atn_state> const& target)
			: transition(transition_type::epsilon, target)
		{
		}
	};

	class precedence_transition : public transition
	{
	private:
		const int32_t _precedence;

	public:
		explicit precedence_transition(std::shared_ptr<atn_state> const& target, int32_t precedence)
			: transition(transition_type::epsilon, target)
			, _precedence(precedence)
		{
		}

	public:
		int32_t precedence() const
		{
			return _precedence;
		}

		std::shared_ptr<semantic_context::precedence_predicate> predicate() const
		{
			return std::make_shared<semantic_context::precedence_predicate>(precedence());
		}
	};

}
}
