// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "atn_type.hpp"
#include "../tree/parse_tree.hpp"

namespace antlr4 {

namespace dfa { class dynamic_dfa; }

namespace atn {

	class atn_state;
	class decision_state;
	class lexer_action;
	class prediction_context;
	class rule_start_state;
	class rule_stop_state;
	class tokens_start_state;

	class grammar_atn
	{
	private:
		class private_data;

		friend class atn_deserializer;

	private:
		const atn_type _grammar_type;
		const int32_t _max_token_type;

		std::vector<std::shared_ptr<atn_state>> _states;
		std::vector<std::shared_ptr<decision_state>> _decisions;
		std::vector<std::shared_ptr<rule_start_state>> _rule_start_states;
		std::vector<std::shared_ptr<rule_stop_state>> _rule_stop_states;
		std::vector<std::shared_ptr<tokens_start_state>> _mode_start_states;

		const std::unique_ptr<private_data> _data;

		std::vector<int32_t> _rule_to_token_type;
		std::vector<std::shared_ptr<dfa::dynamic_dfa>> _decision_dfa;
		std::vector<std::shared_ptr<dfa::dynamic_dfa>> _mode_dfa;

		std::vector<std::shared_ptr<lexer_action>> _lexer_actions;

	public:
		grammar_atn(atn_type grammar_type, int32_t max_token_type);
		~grammar_atn();

	public:
		atn_type grammar_type() const
		{
			return _grammar_type;
		}

		int32_t max_token_type() const
		{
			return _max_token_type;
		}

		size_t context_cache_size() const;

		void clear_dfa();
		std::shared_ptr<prediction_context> cached_context(std::shared_ptr<prediction_context> const& context);

		std::vector<std::shared_ptr<dfa::dynamic_dfa>> const& decision_to_dfa() const
		{
			return _decision_dfa;
		}

		std::vector<int32_t> next_tokens(std::shared_ptr<atn_state> const& state, std::shared_ptr<prediction_context> const& context) const;
		std::vector<int32_t> next_tokens(std::shared_ptr<atn_state> const& state) const;

		void add_state(std::shared_ptr<atn_state> const& state);
		void remove_state(std::shared_ptr<atn_state> const& state);
		void define_mode(std::wstring const& name, std::shared_ptr<tokens_start_state> const& state);
		void define_decision_state(std::shared_ptr<decision_state> const& state);

		std::vector<std::shared_ptr<atn_state>> const& states() const
		{
			return _states;
		}

		std::vector<std::shared_ptr<decision_state>> const& decisions() const
		{
			return _decisions;
		}

		std::shared_ptr<decision_state> decision(size_t decision) const
		{
			return decisions()[decision];
		}

		std::vector<std::shared_ptr<rule_start_state>> const& rule_start_states() const
		{
			return _rule_start_states;
		}

		std::vector<std::shared_ptr<rule_stop_state>> const& rule_stop_states() const
		{
			return _rule_stop_states;
		}

		std::vector<int32_t> expected_tokens(size_t state_number, std::shared_ptr<tree::rule_node> const& context);
	};

}
}
