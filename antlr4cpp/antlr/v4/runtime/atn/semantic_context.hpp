// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>
#include <vector>

namespace antlr4 {
namespace atn {

	class semantic_context
	{
	public:
		static const std::shared_ptr<semantic_context> none;

		enum class context_type
		{
			predicate,
			precedence_predicate,
			and_operator,
			or_operator,
		};

	private:
		const context_type _type;

	protected:
		semantic_context(context_type type)
			: _type(type)
		{
		}

	public:
		context_type type() const
		{
			return _type;
		}

	public:
		static std::shared_ptr<semantic_context> combine_and(std::shared_ptr<semantic_context> const& x, std::shared_ptr<semantic_context> const& y);
		static std::shared_ptr<semantic_context> combine_or(std::shared_ptr<semantic_context> const& x, std::shared_ptr<semantic_context> const& y);

	public:
		class predicate;
		class precedence_predicate;
		class and_operator;
		class or_operator;
	};

	class semantic_context::predicate : public semantic_context
	{
	private:
		const int _rule_index;
		const int _predicate_index;
		const bool _context_dependent;

	public:
		predicate(int32_t rule_index, int32_t predicate_index, bool context_dependent)
			: semantic_context(context_type::predicate)
			, _rule_index(rule_index)
			, _predicate_index(predicate_index)
			, _context_dependent(context_dependent)
		{
		}

		int32_t rule_index() const
		{
			return _rule_index;
		}

		int32_t predicate_index() const
		{
			return _predicate_index;
		}

		bool context_dependent() const
		{
			return _context_dependent;
		}
	};

	class semantic_context::precedence_predicate : public semantic_context
	{
	private:
		const int32_t _precedence;

	public:
		precedence_predicate(int precedence)
			: semantic_context(context_type::precedence_predicate)
			, _precedence(precedence)
		{
		}

		int32_t precedence() const
		{
			return _precedence;
		}
	};

	class semantic_context::and_operator : public semantic_context
	{
	private:

	private:
		std::vector<std::shared_ptr<semantic_context>> _operands;

	public:
		and_operator(std::shared_ptr<semantic_context> const& x, std::shared_ptr<semantic_context> const& y);

	public:
		std::vector<std::shared_ptr<semantic_context>> const& operands() const
		{
			return _operands;
		}
	};

	class semantic_context::or_operator : public semantic_context
	{
	private:

	private:
		std::vector<std::shared_ptr<semantic_context>> _operands;

	public:
		or_operator(std::shared_ptr<semantic_context> const& x, std::shared_ptr<semantic_context> const& y);

	public:
		std::vector<std::shared_ptr<semantic_context>> const& operands() const
		{
			return _operands;
		}
	};

	bool operator== (semantic_context const& x, semantic_context const& y);

}
}

namespace std {

	template<>
	struct hash<antlr4::atn::semantic_context>
	{
		size_t operator() (antlr4::atn::semantic_context const& x) const;
	};

}
