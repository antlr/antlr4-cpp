// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <stdexcept>

#include <antlr/v4/runtime/atn/lexer_action.hpp>

namespace antlr4 {
namespace atn {

	namespace {

		class concrete_more_action : public lexer_action::more_action
		{
		private:
			concrete_more_action(concrete_more_action const&) = delete;
			concrete_more_action& operator= (concrete_more_action const&) = delete;

		public:
			explicit concrete_more_action()
			{
			}
		};

		class concrete_pop_mode_action : public lexer_action::pop_mode_action
		{
		private:
			concrete_pop_mode_action(concrete_pop_mode_action const&) = delete;
			concrete_pop_mode_action& operator= (concrete_pop_mode_action const&) = delete;

		public:
			explicit concrete_pop_mode_action()
			{
			}
		};

		class concrete_skip_action : public lexer_action::skip_action
		{
		private:
			concrete_skip_action(concrete_skip_action const&) = delete;
			concrete_skip_action& operator= (concrete_skip_action const&) = delete;

		public:
			explicit concrete_skip_action()
			{
			}
		};

	}

	const std::shared_ptr<lexer_action::more_action> lexer_action::more_action::instance
		= std::make_shared<concrete_more_action>();

	const std::shared_ptr<lexer_action::pop_mode_action> lexer_action::pop_mode_action::instance
		= std::make_shared<concrete_pop_mode_action>();

	const std::shared_ptr<lexer_action::skip_action> lexer_action::skip_action::instance
		= std::make_shared<concrete_skip_action>();

	lexer_action::more_action::more_action()
		: lexer_action(lexer_action_type::more)
	{
	}

	lexer_action::pop_mode_action::pop_mode_action()
		: lexer_action(lexer_action_type::pop_mode)
	{
	}

	lexer_action::skip_action::skip_action()
		: lexer_action(lexer_action_type::skip)
	{
	}

	bool operator== (lexer_action const& /*x*/, lexer_action const& /*y*/)
	{
		throw std::runtime_error("not implemented");
	}

}
}

namespace std {

	using namespace antlr4::atn;

	size_t hash<lexer_action>::operator() (lexer_action const& /*action*/) const
	{
		throw std::runtime_error("not implemented");
	};

}
