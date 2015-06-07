// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <stdexcept>

#include "grammar_atn.hpp"

namespace antlr4 {
namespace atn {

	class grammar_atn::private_data
	{
	};

	grammar_atn::grammar_atn(atn_type grammar_type, int32_t max_token_type)
		: _grammar_type(grammar_type)
		, _max_token_type(max_token_type)
		, _data(std::make_unique<private_data>())
	{
	}

	grammar_atn::~grammar_atn()
	{
	}

	size_t grammar_atn::context_cache_size() const
	{
		throw std::runtime_error("not implemented");
	}

	void grammar_atn::clear_dfa()
	{
		throw std::runtime_error("not implemented");
	}

	std::shared_ptr<prediction_context> grammar_atn::cached_context(std::shared_ptr<prediction_context> const& /*context*/)
	{
		throw std::runtime_error("not implemented");
	}

	std::vector<int32_t> grammar_atn::next_tokens(std::shared_ptr<atn_state> const& /*state*/, std::shared_ptr<prediction_context> const& /*context*/) const
	{
		throw std::runtime_error("not implemented");
	}

	std::vector<int32_t> grammar_atn::next_tokens(std::shared_ptr<atn_state> const& /*state*/) const
	{
		throw std::runtime_error("not implemented");
	}

	void grammar_atn::add_state(std::shared_ptr<atn_state> const& /*state*/)
	{
		throw std::runtime_error("not implemented");
	}

	void grammar_atn::remove_state(std::shared_ptr<atn_state> const& /*state*/)
	{
		throw std::runtime_error("not implemented");
	}

	void grammar_atn::define_mode(std::wstring const& /*name*/, std::shared_ptr<tokens_start_state> const& /*state*/)
	{
		throw std::runtime_error("not implemented");
	}

	void grammar_atn::define_decision_state(std::shared_ptr<decision_state> const& /*state*/)
	{
		throw std::runtime_error("not implemented");
	}

	std::vector<int32_t> grammar_atn::expected_tokens(size_t /*state_number*/, std::shared_ptr<tree::rule_node> const& /*context*/)
	{
		throw std::runtime_error("not implemented");
	}

}
}
