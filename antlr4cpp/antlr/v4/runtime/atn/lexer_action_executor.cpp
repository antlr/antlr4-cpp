#include "stdafx.h"

#include <iterator>

#include <antlr/v4/runtime/atn/lexer_action_executor.hpp>

#include <antlr/v4/runtime/atn/lexer_action.hpp>
#include <antlr/v4/runtime/misc/murmur_hash.hpp>
#include <antlr/v4/runtime/misc/ptr_equal_to.hpp>
#include <antlr/v4/runtime/misc/ptr_hash.hpp>

namespace antlr4 {
namespace runtime {
namespace atn {

	using namespace antlr4::runtime::misc;

	namespace {

		size_t calculate_hash_code(std::vector<std::shared_ptr<lexer_action>> const& actions)
		{
			int32_t hash = murmur_hash::initialize();
			for each (std::shared_ptr<lexer_action> action in actions)
			{
				hash = murmur_hash::update(hash, action, ptr_hash<std::shared_ptr<lexer_action>>());
			}

			return static_cast<size_t>(murmur_hash::finish(hash, actions.size()));
		}

	}

	lexer_action_executor::lexer_action_executor(std::vector<std::shared_ptr<lexer_action>>&& actions)
		: _actions(actions)
		, _hash_code(calculate_hash_code(_actions))
	{
	}

	std::shared_ptr<lexer_action_executor> lexer_action_executor::append(std::shared_ptr<lexer_action_executor> const& executor, std::shared_ptr<lexer_action> const& action)
	{
		if (!executor)
		{
			return std::make_shared<lexer_action_executor>(std::vector<std::shared_ptr<lexer_action>> { action });
		}

		std::vector<std::shared_ptr<lexer_action>> actions;
		actions.reserve(executor->actions().size() + 1);
		std::copy(executor->actions().begin(), executor->actions().end(), std::back_inserter(actions));
		actions.push_back(action);
		return std::make_shared<lexer_action_executor>(std::move(actions));
	}

	std::shared_ptr<lexer_action_executor> lexer_action_executor::fix_offset_before_match(std::shared_ptr<lexer_action_executor> const& executor, size_t offset)
	{
		std::vector<std::shared_ptr<lexer_action>> updated_actions;
		for (size_t i = 0; i < executor->actions().size(); i++)
		{
			if (executor->actions()[i]->position_dependent() && executor->actions()[i]->action_type() != lexer_action::lexer_action_type::indexed_custom)
			{
				if (updated_actions.empty())
				{
					updated_actions.insert(updated_actions.end(), executor->actions().begin(), executor->actions().end());
				}

				updated_actions[i] = std::make_shared<lexer_action::indexed_custom_action>(offset, updated_actions[i]);
			}
		}

		if (updated_actions.empty())
		{
			return executor;
		}

		return std::make_shared<lexer_action_executor>(std::move(updated_actions));
	}

	bool operator==(lexer_action_executor const& x, lexer_action_executor const& y)
	{
		if (&x == &y)
		{
			return true;
		}

		if (x._hash_code != y._hash_code)
		{
			return false;
		}

		if (x.actions().size() != y.actions().size())
		{
			return false;
		}

		return std::equal(x.actions().begin(), x.actions().end(), y.actions().begin(), misc::ptr_equal_to<std::shared_ptr<lexer_action>>());
	}

}
}
}
