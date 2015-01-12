#pragma once

#include "parse_tree_visitor.hpp"

namespace antlr4 {
namespace runtime {
namespace tree {

	template<typename _TResult>
	void parse_tree_visitor_impl<_TResult>::visit(parse_tree const& node)
	{
		node.accept(*this);
	}

	template<typename _TResult>
	void parse_tree_visitor_impl<_TResult>::visit(terminal_node const& /*node*/)
	{
		_result = default_result();
	}

	template<typename _TResult>
	void parse_tree_visitor_impl<_TResult>::visit(error_node const& /*node*/)
	{
		_result = default_result();
	}

	template<typename _TResult>
	void parse_tree_visitor_impl<_TResult>::visit_children(rule_node const& node)
	{
		_TResult result(default_result());
		size_t size = node.size();
		for (size_t i = 0; i < size; i++)
		{
			if (!should_visit_next_child(node, result))
			{
				break;
			}

			std::shared_ptr<parse_tree> child = node.child(i);
			child->accept(*this);
			result = aggregate_result(result, _result);
		}

		_result = result;
	}

	template<typename _TResult>
	_TResult parse_tree_visitor_impl<_TResult>::default_result()
	{
		return _TResult();
	}

	template<typename _TResult>
	_TResult parse_tree_visitor_impl<_TResult>::aggregate_result(result_param_type /*aggregate*/, result_param_type next_result)
	{
		return next_result;
	}

	template<typename _TResult>
	bool parse_tree_visitor_impl<_TResult>::should_visit_next_child(rule_node const& /*node*/, result_param_type /*current_result*/)
	{
		return true;
	}

}
}
}
