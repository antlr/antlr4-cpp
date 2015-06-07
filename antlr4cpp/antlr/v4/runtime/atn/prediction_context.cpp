// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <cassert>
#include <deque>
#include <unordered_map>
#include <unordered_set>

#include <antlr/v4/runtime/atn/prediction_context.hpp>
#include <antlr/v4/runtime/atn/prediction_context_cache.hpp>
#include <antlr/v4/runtime/misc/murmur_hash.hpp>

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) )
#endif

namespace antlr4 {
namespace atn {

	namespace {

		using misc::murmur_hash;
		typedef prediction_context_cache::identity_commutative_prediction_context_operands identity_commutative_prediction_context_operands;

		int32_t calculate_empty_hash_code();

		struct concrete_prediction_context : prediction_context
		{
		public:
			explicit concrete_prediction_context()
				: prediction_context(calculate_empty_hash_code())
			{
			}

			concrete_prediction_context(std::shared_ptr<prediction_context> const& parent, int32_t return_state)
				: prediction_context(parent, return_state)
			{
			}

			concrete_prediction_context(std::vector<std::shared_ptr<prediction_context>>&& parents, std::vector<int32_t>&& return_states)
				: prediction_context(std::move(parents), std::move(return_states))
			{
			}
		};

		bool context_equal(prediction_context const& x, prediction_context const& y, std::deque<std::shared_ptr<prediction_context>>& self_work_list, std::deque<std::shared_ptr<prediction_context>>& other_work_list)
		{
			size_t self_size = x.size();
			if (!self_size)
			{
				// only one instance of each case of the empty operands exists, so we just need to compare pointers
				if (!(&x == &y))
				{
					return false;
				}

				return true;
			}

			size_t other_size = y.size();
			if (self_size != other_size)
			{
				return false;
			}

			std::hash<prediction_context> hasher;
			for (size_t i = 0; i < self_size; i++)
			{
				if (x.return_state(i) != y.return_state(i))
				{
					return false;
				}

				auto self_parent = x.parent(i);
				auto other_parent = y.parent(i);
				if (self_parent == other_parent)
				{
					continue;
				}

				if (hasher(*self_parent) != hasher(*other_parent))
				{
					return false;
				}

				self_work_list.push_back(std::move(self_parent));
				other_work_list.push_back(std::move(other_parent));
			}

			return true;
		}

		bool context_equal(prediction_context const& x, prediction_context const& y)
		{
			std::unordered_set<identity_commutative_prediction_context_operands> visited;
			std::deque<std::shared_ptr<prediction_context>> self_work_list;
			std::deque<std::shared_ptr<prediction_context>> other_work_list;

			if (!context_equal(x, y, self_work_list, other_work_list))
			{
				return false;
			}

			while (!self_work_list.empty())
			{
				identity_commutative_prediction_context_operands operands(std::move(self_work_list.back()), std::move(other_work_list.back()));
				self_work_list.pop_back();
				other_work_list.pop_back();
				auto result = visited.insert(operands);
				if (!result.second)
				{
					continue;
				}

				if (!context_equal(*operands.x(), *operands.y(), self_work_list, other_work_list))
				{
					return false;
				}
			}

			return true;
		}

		const int32_t initial_hash = 1;

		int32_t calculate_empty_hash_code()
		{
			int32_t hash = murmur_hash::initialize(initial_hash);
			hash = murmur_hash::finish(hash, 0);
			return hash;
		}

		int32_t calculate_hash_code(std::shared_ptr<prediction_context> const& parent, int32_t return_state)
		{
			int32_t hash = murmur_hash::initialize(initial_hash);
			hash = murmur_hash::update(hash, parent);
			hash = murmur_hash::update(hash, return_state);
			hash = murmur_hash::finish(hash, 2);
			return hash;
		}

		int32_t calculate_hash_code(std::vector<std::shared_ptr<prediction_context>> const& parents, std::vector<int32_t> const& return_states)
		{
			int32_t hash = murmur_hash::initialize(initial_hash);
			for each (std::shared_ptr<prediction_context> const& parent in parents)
			{
				hash = murmur_hash::update(hash, parent);
			}

			for each (int32_t return_state in return_states)
			{
				hash = murmur_hash::update(hash, return_state);
			}

			hash = murmur_hash::finish(hash, 2 * parents.size());
			return hash;
		}

		// The default hash function for pointers is reference equality. Since C++ does not move objects in memory, we
		// don't need to provide the specialized hash mechanism here to avoid the computation of an identity hash code
		// like the Java code needs.
		typedef std::unordered_map<std::shared_ptr<prediction_context>, std::shared_ptr<prediction_context>> identity_map;

		std::shared_ptr<prediction_context> append_context_impl(std::shared_ptr<prediction_context> const& context, std::shared_ptr<prediction_context> const& suffix, identity_map& visited)
		{
			if (suffix->is_empty())
			{
				if (suffix->is_empty_local())
				{
					if (context->has_empty())
					{
						return prediction_context::empty_local;
					}

					// This is also not implemented in the Java code.
					throw std::runtime_error("what to do here?");
				}

				return context;
			}

			if (suffix->size() != 1)
			{
				throw std::runtime_error("Appending a tree suffix is not yet supported.");
			}

			auto result_it = visited.find(context);
			if (result_it == visited.end())
			{
				std::shared_ptr<prediction_context> result;
				if (context->is_empty())
				{
					result = suffix;
				}
				else
				{
					size_t parent_count = context->size();
					if (context->has_empty())
					{
						parent_count--;
					}

					std::vector<std::shared_ptr<prediction_context>> updated_parents(parent_count);
					std::vector<int32_t> updated_return_states(parent_count);
					for (size_t i = 0; i < parent_count; i++)
					{
						// this loop could be improved with access to the prediction_context::return_states
						updated_return_states[i] = context->return_state(i);
					}

					for (size_t i = 0; i < parent_count; i++)
					{
						updated_parents[i] = append_context_impl(context->parent(i), suffix, visited);
					}

					result = std::make_shared<concrete_prediction_context>(std::move(updated_parents), std::move(updated_return_states));

					if (context->has_empty())
					{
						prediction_context_cache context_cache(prediction_context_cache::uncached());
						result = prediction_context::join(result, suffix, context_cache);
					}
				}

				result_it = visited.insert(std::make_pair(context, result)).first;
			}

			return result_it->second;
		}

	}

	const std::shared_ptr<prediction_context> prediction_context::empty_local(std::make_shared<concrete_prediction_context>());
	const std::shared_ptr<prediction_context> prediction_context::empty_full(std::make_shared<concrete_prediction_context>());

	prediction_context::prediction_context(int32_t cached_hash_code)
		: cached_hash_code(cached_hash_code)
	{
	}

	prediction_context::prediction_context(std::shared_ptr<prediction_context> const& parent, int32_t return_state)
		: cached_hash_code(calculate_hash_code(parent, return_state))
		, parents(1)
		, return_states(1)
	{
		parents[0] = parent;
		return_states[0] = return_state;
	}

	prediction_context::prediction_context(std::vector<std::shared_ptr<prediction_context>>&& parents, std::vector<int32_t>&& return_states)
		: cached_hash_code(calculate_hash_code(parents, return_states))
		, parents(parents)
		, return_states(return_states)
	{
		assert(this->parents.size() == this->return_states.size());
	}

	size_t prediction_context::find_return_state(int32_t return_state) const
	{
		auto bound = std::lower_bound(std::cbegin(return_states), std::cend(return_states), return_state);
		return static_cast<size_t>(bound - return_states.begin());
	}

	std::shared_ptr<prediction_context> prediction_context::add_empty_context(std::shared_ptr<prediction_context> const& context)
	{
		if (context->has_empty())
		{
			return context;
		}

		auto parents(context->parents);
		parents.push_back(empty_full);
		auto return_states(context->return_states);
		return_states.push_back(empty_full_state_key);

		return std::make_shared<concrete_prediction_context>(std::move(parents), std::move(return_states));
	}

	std::shared_ptr<prediction_context> prediction_context::remove_empty_context(std::shared_ptr<prediction_context> const& context)
	{
		if (!context->has_empty())
		{
			return context;
		}

		auto parents(context->parents);
		parents.pop_back();
		auto return_states(context->return_states);
		return_states.pop_back();

		return std::make_shared<concrete_prediction_context>(std::move(parents), std::move(return_states));
	}

	std::shared_ptr<prediction_context> prediction_context::append_context(std::shared_ptr<prediction_context> const& context, int32_t return_context, prediction_context_cache& context_cache)
	{
		return append_context(context, get_child(empty_full, return_context), context_cache);
	}

	std::shared_ptr<prediction_context> prediction_context::append_context(std::shared_ptr<prediction_context> const& context, std::shared_ptr<prediction_context> const& suffix, prediction_context_cache& context_cache)
	{
		if (context->is_empty())
		{
			return suffix;
		}

		if (context->size() == 1)
		{
			return context_cache.get_child(append_context(context->parents[0], suffix, context_cache), context->return_states[0]);
		}

		identity_map visited;
		return append_context_impl(context, suffix, visited);
	}

	std::shared_ptr<prediction_context> prediction_context::get_child(std::shared_ptr<prediction_context> const& context, int32_t return_state)
	{
		return std::make_shared<concrete_prediction_context>(context, return_state);
	}

	std::shared_ptr<prediction_context> prediction_context::from_rule_context(std::shared_ptr<grammar_atn> const& /*atn*/, std::shared_ptr<rule_context> const& /*outer_context*/, bool /*full_context*/)
	{
		throw std::runtime_error("Not implemented");
		//if (outer_context->is_empty())
		//{
		//	return full_context ? empty_full : empty_local;
		//}

		//std::shared_ptr<prediction_context> parent;
		//if (outer_context->parent())
		//{
		//	parent = from_rule_context(atn, outer_context->parent(), full_context);
		//}
		//else
		//{
		//	parent = full_context ? empty_full : empty_local;
		//}

		//auto state = atn->states().find(outer_context->invoking_state);
		//auto transition = static_cast<rule_transition>(state->transition(0));
		//return get_child(parent, transition->follow_state()->state_number());
	}

	std::shared_ptr<prediction_context> prediction_context::join(std::shared_ptr<prediction_context> const& context0, std::shared_ptr<prediction_context> const& context1, prediction_context_cache& context_cache)
	{
		if (context0 == context1)
		{
			return context0;
		}

		if (context0->is_empty())
		{
			return context0->is_empty_local() ? context0 : add_empty_context(context1);
		}
		else if (context1->is_empty())
		{
			return context1->is_empty_local() ? context1 : add_empty_context(context0);
		}

		const size_t context0_size = context0->size();
		const size_t context1_size = context1->size();
		if (context0_size == 1 && context1_size == 1 && context0->return_state(0) == context1->return_state(0))
		{
			std::shared_ptr<prediction_context> merged = context_cache.join(context0->parent(0), context1->parent(0));
			if (merged == context0->parent(0))
			{
				return context0;
			}
			else if (merged == context1->parent(0))
			{
				return context1;
			}
			else
			{
				return get_child(merged, context0->return_state(0));
			}
		}

		size_t count = 0;
		std::vector<std::shared_ptr<prediction_context>> parents_list;
		std::vector<int32_t> return_states_list;
		parents_list.reserve(context0_size + context1_size);
		return_states_list.reserve(context0_size + context1_size);
		size_t left_index = 0;
		size_t right_index = 0;
		bool can_return_left = true;
		bool can_return_right = true;
		while (left_index < context0_size && right_index < context1_size)
		{
			if (context0->return_state(left_index) == context1->return_state(right_index))
			{
				parents_list.push_back(context_cache.join(context0->parent(left_index), context1->parent(right_index)));
				return_states_list.push_back(context0->return_state(left_index));
				can_return_left &= parents_list[count] == context0->parent(left_index);
				can_return_right &= parents_list[count] == context1->parent(right_index);
				left_index++;
				right_index++;
			}
			else if (context0->return_state(left_index) < context1->return_state(right_index))
			{
				parents_list.push_back(context0->parent(left_index));
				return_states_list.push_back(context0->return_state(left_index));
				can_return_right = false;
				left_index++;
			}
			else
			{
				assert(context1->return_state(right_index) < context0->return_state(left_index));
				parents_list.push_back(context1->parent(right_index));
				return_states_list.push_back(context1->return_state(right_index));
				can_return_left = false;
				right_index++;
			}

			count++;
		}

		while (left_index < context0_size)
		{
			parents_list.push_back(context0->parent(left_index));
			return_states_list.push_back(context0->return_state(left_index));
			left_index++;
			can_return_right = false;
			count++;
		}

		while (right_index < context1_size)
		{
			parents_list.push_back(context1->parent(right_index));
			return_states_list.push_back(context1->return_state(right_index));
			right_index++;
			can_return_left = false;
			count++;
		}

		if (can_return_left)
		{
			return context0;
		}
		else if (can_return_right)
		{
			return context1;
		}

		parents_list.shrink_to_fit();
		return_states_list.shrink_to_fit();

		if (parents_list.empty())
		{
			// if one of them was `empty_local`, it would be empty and handled at the beginning of the method
			return empty_full;
		}
		else
		{
			return std::make_shared<concrete_prediction_context>(std::move(parents_list), std::move(return_states_list));
		}
	}

	bool operator== (prediction_context const& x, prediction_context const& y)
	{
		if (&x == &y)
			return true;

		if (x.cached_hash_code != y.cached_hash_code)
			return false;

		return context_equal(x, y);
	}

}
}
