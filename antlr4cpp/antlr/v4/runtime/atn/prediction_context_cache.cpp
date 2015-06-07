// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <unordered_map>

#include <antlr/v4/runtime/atn/prediction_context.hpp>
#include <antlr/v4/runtime/atn/prediction_context_cache.hpp>

namespace antlr4 {
namespace atn {

	namespace {

		class prediction_context_and_int
		{
			prediction_context_and_int() = delete;
			prediction_context_and_int(prediction_context_and_int const&) = delete;
			prediction_context_and_int& operator= (prediction_context_and_int const&) = delete;

		public:
			const std::shared_ptr<prediction_context> context;
			const int32_t value;

		public:
			prediction_context_and_int(prediction_context_and_int&& other)
				: context(std::move(other.context))
				, value(other.value)
			{
			}

			prediction_context_and_int(std::shared_ptr<prediction_context> const& context, int32_t value)
				: context(context)
				, value(value)
			{
			}
		};

		bool operator== (prediction_context_and_int const& x, prediction_context_and_int const& y)
		{
			if (&x == &y)
			{
				return true;
			}

			return x.value == y.value
				&& (x.context == y.context || (x.context && y.context && *x.context == *y.context));
		}

	}

}
}

namespace std {

	template<>
	struct hash<antlr4::atn::prediction_context_and_int>
	{
		size_t operator() (antlr4::atn::prediction_context_and_int const& value) const
		{
			size_t hash = 5;
			hash = 7 * hash + (value.context ? std::hash<antlr4::atn::prediction_context>()(*value.context) : 0);
			hash = 7 * hash + value.value;
			return hash;
		}
	};

}

namespace antlr4 {
namespace atn {

	class prediction_context_cache::data
	{
		data(data const&) = delete;
		data& operator= (data const&) = delete;

	public:
		std::unordered_map<std::shared_ptr<prediction_context>, std::shared_ptr<prediction_context>> contexts;
		std::unordered_map<prediction_context_and_int, std::shared_ptr<prediction_context>> child_contexts;
		std::unordered_map<identity_commutative_prediction_context_operands, std::shared_ptr<prediction_context>> join_contexts;

	public:
		data()
		{
		}
	};

	prediction_context_cache::prediction_context_cache(bool enable_cache)
		: private_data(enable_cache ? std::move(std::make_unique<prediction_context_cache::data>()) : nullptr)
	{
	}

	prediction_context_cache::prediction_context_cache(prediction_context_cache && cache)
		: private_data(std::move(cache.private_data))
	{
	}

	prediction_context_cache::~prediction_context_cache()
	{
	}

	prediction_context_cache prediction_context_cache::uncached()
	{
		return prediction_context_cache();
	}

	std::shared_ptr<prediction_context> prediction_context_cache::get_as_cached(std::shared_ptr<prediction_context> const& context)
	{
		if (!private_data)
		{
			return context;
		}

		auto result = private_data->contexts.insert(std::make_pair(context, context)).first;
		return result->second;
	}

	std::shared_ptr<prediction_context> prediction_context_cache::get_child(std::shared_ptr<prediction_context> const& context, int return_state)
	{
		if (!private_data)
		{
			return prediction_context::get_child(context, return_state);
		}

		prediction_context_and_int operands(context, return_state);
		auto result = private_data->child_contexts.find(operands);
		if (result == private_data->child_contexts.end())
		{
			auto child_context = get_as_cached(prediction_context::get_child(context, return_state));
			result = private_data->child_contexts.insert(std::make_pair(std::move(operands), std::move(child_context))).first;
		}

		return result->second;
	}

	std::shared_ptr<prediction_context> prediction_context_cache::join(std::shared_ptr<prediction_context> const& x, std::shared_ptr<prediction_context> const& y)
	{
		if (!private_data)
		{
			return prediction_context::join(x, y, *this);
		}

		auto operands = identity_commutative_prediction_context_operands(std::shared_ptr<prediction_context>(x), std::shared_ptr<prediction_context>(y));
		auto result = private_data->join_contexts.find(operands);
		if (result != private_data->join_contexts.end())
		{
			return result->second;
		}

		auto join_context = get_as_cached(prediction_context::join(x, y, *this));
		result = private_data->join_contexts.insert(std::make_pair(std::move(operands), std::move(join_context))).first;
		return result->second;
	}

}
}
