/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
#pragma once

namespace antlr4 {
namespace runtime {
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
}
