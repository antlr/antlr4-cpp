// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>

namespace antlr4 {

	namespace atn {
		class lexer_action_executor;
	}

namespace dfa {

	class accept_state_information
	{
	private:
		size_t _prediction;
		std::shared_ptr<atn::lexer_action_executor> _lexer_action_executor;

	public:
		accept_state_information(size_t prediction)
			: _prediction(prediction)
			, _lexer_action_executor(nullptr)
		{
		}

		accept_state_information(size_t prediction, std::shared_ptr<atn::lexer_action_executor> const& lexer_action_executor)
			: _prediction(prediction)
			, _lexer_action_executor(lexer_action_executor)
		{
		}

	public:
		size_t prediction() const
		{
			return _prediction;
		}

		std::shared_ptr<atn::lexer_action_executor> const& lexer_action_executor() const
		{
			return _lexer_action_executor;
		}
	};

}
}
