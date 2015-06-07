// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>
#include <string>
#include "../misc/visitor.hpp"

namespace antlr4 {

	class token;

namespace tree {

	class error_node;
	class terminal_node;
	class parse_tree_listener;
	class parse_tree_visitor;

	class parse_tree
	{
	protected:
		virtual ~parse_tree() { }

	public:
		enum class tree_type
		{
			rule,
			terminal,
			error,
		};

	public:
		// used to avoid certain cases of dynamic_cast
		virtual tree_type type() const = 0;

	public:
		virtual std::shared_ptr<parse_tree> parent() const = 0;
		virtual std::shared_ptr<parse_tree> child(size_t index) const = 0;
		virtual size_t size() const = 0;

		virtual std::pair<size_t, size_t> source_interval() const = 0;
		virtual std::wstring text() const = 0;
		virtual std::wstring tree_text() const = 0;

		virtual void accept(parse_tree_visitor& visitor) const = 0;
	};

	class rule_node : public parse_tree
	{
	private:
		std::shared_ptr<rule_node> _parent;

	public:
		rule_node(std::shared_ptr<rule_node> const& parent)
			: _parent(parent)
		{
		}

	public:
		virtual void enter_rule(parse_tree_listener& /*listener*/) const { }
		virtual void exit_rule(parse_tree_listener& /*listener*/) const { }

	public:
		virtual tree_type type() const override final
		{
			return tree_type::rule;
		}

	public:
		virtual std::shared_ptr<parse_tree> parent() const override
		{
			return _parent;
		}

		virtual std::shared_ptr<parse_tree> child(size_t /*index*/) const override
		{
			return nullptr;
		}

		virtual size_t size() const override
		{
			return 0;
		}

		virtual std::pair<size_t, size_t> source_interval() const override
		{
			return std::make_pair(~size_t(), ~0);
		}

		virtual std::wstring text() const override;
		virtual std::wstring tree_text() const override;

		virtual void accept(parse_tree_visitor& visitor) const override;
	};

	class terminal_node : public parse_tree
	{
	private:
		const std::shared_ptr<token> _token;
		const std::shared_ptr<rule_node> _parent;

	public:
		terminal_node(std::shared_ptr<token> const& token, std::shared_ptr<rule_node> const& parent)
			: _token(token)
			, _parent(parent)
		{
		}

	public:
		std::shared_ptr<token> const& token() const
		{
			return _token;
		}

	public:
		virtual tree_type type() const override
		{
			return tree_type::terminal;
		}

	public:
		virtual std::shared_ptr<parse_tree> parent() const override
		{
			return _parent;
		}

		virtual std::shared_ptr<parse_tree> child(size_t /*index*/) const override
		{
			return nullptr;
		}

		virtual size_t size() const override
		{
			return 0;
		}

		virtual std::pair<size_t, size_t> source_interval() const override;
		virtual std::wstring text() const override;
		virtual std::wstring tree_text() const override;

		virtual void accept(parse_tree_visitor& visitor) const override;
	};

	class error_node : public terminal_node
	{
	public:
		error_node(std::shared_ptr<antlr4::token> const& token, std::shared_ptr<rule_node> const& parent)
			: terminal_node(token, parent)
		{
		}

	public:
		virtual tree_type type() const override final
		{
			return tree_type::error;
		}

	public:
		virtual void accept(parse_tree_visitor& visitor) const override;
	};

}
}
