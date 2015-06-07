// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

namespace antlr4 {
namespace tree {

	class error_node;
	class parse_tree;
	class rule_node;
	class terminal_node;

	class parse_tree_listener
	{
	protected:
		virtual ~parse_tree_listener() { }

	public:
		virtual void enter_node(rule_node const& node) = 0;
		virtual void exit_node(rule_node const& node) = 0;
		virtual void visit(terminal_node const& node) = 0;
		virtual void visit(error_node const& node) = 0;
	};

}
}
