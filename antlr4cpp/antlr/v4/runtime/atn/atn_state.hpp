// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace antlr4 {
namespace atn {

	class transition;

	class atn_state abstract
	{
	public:
		enum class atn_state_type
		{
			invalid,
			basic,
			rule_start,
			block_start,
			plus_block_start,
			star_block_start,
			token_start,
			rule_stop,
			block_end,
			star_loop_back,
			star_loop_entry,
			plus_loop_back,
			loop_end,
		};

	private:
		const atn_state_type _state_type;
		const size_t _state_number;
		const size_t _rule_index;

		bool _epsilon_only_transitions;
		bool _optimized;

		std::vector<std::shared_ptr<transition>> _transitions;
		std::vector<std::shared_ptr<transition>> _optimized_transitions;

	public:
		static const size_t invalid_state_number = ~static_cast<size_t>(0);

	protected:
		atn_state(atn_state_type state_type, size_t state_number, size_t rule_index)
			: _state_type(state_type)
			, _state_number(state_number)
			, _rule_index(rule_index)
			, _epsilon_only_transitions()
		{
		}

	public:
		atn_state_type state_type() const
		{
			return _state_type;
		}

		size_t state_number() const
		{
			return _state_number;
		}

		size_t rule_index() const
		{
			return _rule_index;
		}

		bool only_has_epsilon_transitions() const
		{
			return _epsilon_only_transitions;
		}

		bool optimized() const
		{
			return _optimized;
		}

		std::vector<std::shared_ptr<atn::transition>> const& transitions() const
		{
			return _transitions;
		}

		std::shared_ptr<transition> const& transition(size_t index) const
		{
			return transitions()[index];
		}

		std::vector<std::shared_ptr<atn::transition>> const& optimized_transitions() const
		{
			return optimized() ? _optimized_transitions : _transitions;
		}

		std::shared_ptr<atn::transition> const& optimized_transition(size_t index) const
		{
			return optimized_transitions()[index];
		}

		size_t non_stop_state_number() const
		{
			return state_type() == atn_state_type::rule_stop ? invalid_state_number : state_number();
		}

	public:
		void add_transition(std::shared_ptr<atn::transition> const& transition);
		void add_transition(size_t index, std::shared_ptr<atn::transition> const& transition);
		void set_transition(size_t index, std::shared_ptr<atn::transition> const& transition);
		std::shared_ptr<atn::transition> remove_transition(size_t index);

		void add_optimized_transition(size_t index, std::shared_ptr<atn::transition> const& transition);
		void set_optimized_transition(size_t index, std::shared_ptr<atn::transition> const& transition);
		std::shared_ptr<atn::transition> remove_optimized_transition(size_t index);
	};

	inline bool operator== (atn_state const& x, atn_state const& y)
	{
		return x.state_number() == y.state_number();
	}

	class basic_state;
	class block_end_state;
	class decision_state;
	class block_start_state;
	class basic_block_start_state;
	class plus_block_start_state;
	class star_block_start_state;
	class plus_loopback_state;
	class star_loop_entry_state;
	class tokens_start_state;
	class loop_end_state;
	class rule_start_state;
	class rule_stop_state;
	class star_loopback_state;

	class basic_state final : public atn_state
	{
	public:
		basic_state(size_t state_number, size_t rule_index)
			: atn_state(atn_state_type::basic, state_number, rule_index)
		{
		}
	};

	class block_end_state : public atn_state
	{
	private:
		std::shared_ptr<block_start_state> _start_state;

	public:
		block_end_state(size_t state_number, size_t rule_index)
			: atn_state(atn_state_type::block_end, state_number, rule_index)
		{
		}

	public:
		std::shared_ptr<block_start_state> const& start_state() const
		{
			return _start_state;
		}

		void start_state(std::shared_ptr<block_start_state> const& value)
		{
			_start_state = value;
		}
	};

	class decision_state abstract : public atn_state
	{
	private:
		size_t _decision;
		bool _greedy;
		bool _sll;

	protected:
		decision_state(atn_state_type state_type, size_t state_number, size_t rule_index)
			: atn_state(state_type, state_number, rule_index)
			, _decision(~static_cast<size_t>(0))
			, _greedy(true)
			, _sll(false)
		{
		}

	public:
		size_t decision() const
		{
			return _decision;
		}

		void decision(size_t value)
		{
			_decision = value;
		}

		bool greedy() const
		{
			return _greedy;
		}

		void greedy(bool value)
		{
			_greedy = value;
		}

		bool sll() const
		{
			return _sll;
		}

		void sll(bool value)
		{
			_sll = value;
		}
	};

	class block_start_state abstract : public decision_state
	{
	private:
		std::shared_ptr<block_end_state> _end_state;

	protected:
		block_start_state(atn_state_type state_type, size_t state_number, size_t rule_index)
			: decision_state(state_type, state_number, rule_index)
		{
		}

	public:
		std::shared_ptr<block_end_state> const& end_state() const
		{
			return _end_state;
		}

		void end_state(std::shared_ptr<block_end_state> const& value)
		{
			_end_state = value;
		}
	};

	class basic_block_start_state final : public block_start_state
	{
	public:
		basic_block_start_state(size_t state_number, size_t rule_index)
			: block_start_state(atn_state_type::block_start, state_number, rule_index)
		{
		}
	};

	class plus_block_start_state final : public block_start_state
	{
	private:
		std::shared_ptr<plus_loopback_state> _loopback_state;

	public:
		plus_block_start_state(size_t state_number, size_t rule_index)
			: block_start_state(atn_state_type::plus_block_start, state_number, rule_index)
		{
		}

	public:
		std::shared_ptr<plus_loopback_state> const& loopback_state() const
		{
			return _loopback_state;
		}

		void loopback_state(std::shared_ptr<plus_loopback_state> const& value)
		{
			_loopback_state = value;
		}
	};

	class star_block_start_state final : public block_start_state
	{
	public:
		star_block_start_state(size_t state_number, size_t rule_index)
			: block_start_state(atn_state_type::star_block_start, state_number, rule_index)
		{
		}
	};

	class plus_loopback_state final : public decision_state
	{
	public:
		plus_loopback_state(size_t state_number, size_t rule_index)
			: decision_state(atn_state_type::plus_loop_back, state_number, rule_index)
		{
		}
	};

	class star_loop_entry_state final : public decision_state
	{
	private:
		std::shared_ptr<star_loopback_state> _loopback_state;
		bool _precedence_rule_decision;

	public:
		star_loop_entry_state(size_t state_number, size_t rule_index)
			: decision_state(atn_state_type::star_loop_entry, state_number, rule_index)
			, _precedence_rule_decision()
		{
		}

	public:
		std::shared_ptr<star_loopback_state> const& loopback_state() const
		{
			return _loopback_state;
		}

		void loopback_state(std::shared_ptr<star_loopback_state> const& value)
		{
			_loopback_state = value;
		}

		bool precedence_rule_decision() const
		{
			return _precedence_rule_decision;
		}

		void precedence_rule_decision(bool value)
		{
			_precedence_rule_decision = value;
		}
	};

	class tokens_start_state final : public decision_state
	{
	public:
		tokens_start_state(size_t state_number, size_t rule_index)
			: decision_state(atn_state_type::token_start, state_number, rule_index)
		{
		}
	};

	class loop_end_state final : public atn_state
	{
	private:
		std::shared_ptr<atn_state> _loopback_state;

	public:
		loop_end_state(size_t state_number, size_t rule_index)
			: atn_state(atn_state_type::loop_end, state_number, rule_index)
		{
		}

	public:
		std::shared_ptr<atn_state> const& loopback_state() const
		{
			return _loopback_state;
		}

		void loopback_state(std::shared_ptr<atn_state> const& value)
		{
			_loopback_state = value;
		}
	};

	class rule_start_state final : public atn_state
	{
	private:
		std::shared_ptr<rule_stop_state> _stop_state;
		bool _precedence_rule;
		bool _left_factored;

	public:
		rule_start_state(size_t state_number, size_t rule_index)
			: atn_state(atn_state_type::rule_start, state_number, rule_index)
			, _precedence_rule(false)
			, _left_factored(false)
		{
		}

		std::shared_ptr<rule_stop_state> const& stop_state() const
		{
			return _stop_state;
		}

		void stop_state(std::shared_ptr<rule_stop_state> const& value)
		{
			_stop_state = value;
		}

		bool precedence_rule() const
		{
			return _precedence_rule;
		}

		void precedence_rule(bool value)
		{
			_precedence_rule = value;
		}

		bool left_factored() const
		{
			return _left_factored;
		}

		void left_factored(bool value)
		{
			_left_factored = value;
		}
	};

	class rule_stop_state final : public atn_state
	{
	public:
		rule_stop_state(size_t state_number, size_t rule_index)
			: atn_state(atn_state_type::rule_stop, state_number, rule_index)
		{
		}
	};

	class star_loopback_state final : public atn_state
	{
	public:
		star_loopback_state(size_t state_number, size_t rule_index)
			: atn_state(atn_state_type::star_loop_back, state_number, rule_index)
		{
		}

	public:
		std::shared_ptr<star_loop_entry_state> loop_entry_state() const;
	};

}
}

namespace std {

	template<>
	class hash<antlr4::atn::atn_state>
	{
		size_t operator() (antlr4::atn::atn_state const& state) const
		{
			return state.state_number();
		}
	};

}
