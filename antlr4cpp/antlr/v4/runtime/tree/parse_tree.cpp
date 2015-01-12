#include "stdafx.h"

#include "parse_tree.hpp"
#include "parse_tree_visitor.hpp"

namespace antlr4 {
namespace runtime {
namespace tree {

	std::wstring rule_node::text() const
	{
		throw std::runtime_error("not implemented");
	}

	std::wstring rule_node::tree_text() const
	{
		throw std::runtime_error("not implemented");
	}

	void rule_node::accept(parse_tree_visitor& visitor) const
	{
		visitor.visit_children(*this);
	}

	void terminal_node::accept(parse_tree_visitor& visitor) const
	{
		visitor.visit(*this);
	}

	void error_node::accept(parse_tree_visitor& visitor) const
	{
		visitor.visit(*this);
	}

}
}
}
