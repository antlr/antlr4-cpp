/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
#pragma once

#include <memory>

namespace antlr4 {
namespace runtime {

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
}
