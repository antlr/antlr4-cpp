// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include "dynamic_dfa.hpp"

#include "../token.hpp"
#include "../atn/atn_state.hpp"
#include "../atn/grammar_atn.hpp"

namespace antlr4 {
namespace dfa {

	using namespace atn;

	namespace {

		// TODO: move these constants to lexer_atn_simulator
		static const int32_t lexer_min_dfa_edge = 0;
		static const int32_t lexer_max_dfa_edge = 127;

	}

	dynamic_dfa::dynamic_dfa(std::shared_ptr<atn_state> const& start_state, size_t decision)
		: _start_state(start_state)
		, _decision(decision)
		, _min_dfa_edge(start_state->atn()->grammar_type() == atn_type::lexer ? lexer_min_dfa_edge : token::eof)
		, _max_dfa_edge(start_state->atn()->grammar_type() == atn_type::lexer ? lexer_max_dfa_edge : start_state->atn()->max_token_type())
	{
	}

}
}
