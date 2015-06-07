// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace antlr4 {

	class rule_context;

namespace atn {

	class grammar_atn;
	class prediction_context_cache;

	class prediction_context
	{
		prediction_context() = delete;
		prediction_context(prediction_context const&) = delete;
		prediction_context& operator=(prediction_context const&) = delete;

	private:
		const int32_t cached_hash_code;
		friend std::hash<prediction_context>;
		friend bool operator== (prediction_context const&, prediction_context const&);

		std::vector<std::shared_ptr<prediction_context>> parents;
		std::vector<int32_t> return_states;

	protected:
		prediction_context(int32_t cached_hash_code);
		prediction_context(std::shared_ptr<prediction_context> const& parent, int32_t return_state);
		prediction_context(std::vector<std::shared_ptr<prediction_context>>&& parents, std::vector<int32_t>&& return_states);

	public:
		static const int32_t empty_local_state_key = ~static_cast<int32_t>(0);
		static const int32_t empty_full_state_key = empty_local_state_key - 1;

		static const std::shared_ptr<prediction_context> empty_local;
		static const std::shared_ptr<prediction_context> empty_full;

	public:
		size_t size() const
		{
			return parents.size();
		}

		int32_t return_state(size_t index) const
		{
			return return_states[index];
		}

		std::shared_ptr<prediction_context> const& parent(size_t index) const
		{
			return parents[index];
		}

		bool is_empty() const
		{
			return parents.empty();
		}

		bool is_empty_local() const
		{
			return this == empty_local.get();
		}

		bool has_empty() const
		{
			auto hashMethod = std::hash<std::shared_ptr<prediction_context>>();
			return is_empty() || return_states[size() - 1] == empty_full_state_key;
		}

		size_t find_return_state(int32_t return_state) const;

		static std::shared_ptr<prediction_context> add_empty_context(std::shared_ptr<prediction_context> const& context);
		static std::shared_ptr<prediction_context> remove_empty_context(std::shared_ptr<prediction_context> const& context);
		static std::shared_ptr<prediction_context> append_context(std::shared_ptr<prediction_context> const& context, int32_t return_context, prediction_context_cache& context_cache);
		static std::shared_ptr<prediction_context> append_context(std::shared_ptr<prediction_context> const& context, std::shared_ptr<prediction_context> const& suffix, prediction_context_cache& context_cache);
		static std::shared_ptr<prediction_context> get_child(std::shared_ptr<prediction_context> const& context, int32_t return_state);

		static std::shared_ptr<prediction_context> from_rule_context(std::shared_ptr<grammar_atn> const& atn, std::shared_ptr<rule_context> const& outer_context, bool full_context = true);
		static std::shared_ptr<prediction_context> join(std::shared_ptr<prediction_context> const& context0, std::shared_ptr<prediction_context> const& context1, prediction_context_cache& context_cache);
	};

	bool operator== (prediction_context const& x, prediction_context const& y);

}
}

namespace std {
	template<>
	struct hash<antlr4::atn::prediction_context> {
		size_t operator()(antlr4::atn::prediction_context const& prediction_context) const
		{
			return static_cast<size_t>(prediction_context.cached_hash_code);
		}
	};
}
