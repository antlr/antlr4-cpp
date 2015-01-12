#pragma once

#include "../misc/param_type.hpp"
#include "../misc/visitor.hpp"

namespace antlr4 {
namespace runtime {
namespace tree {

	class error_node;
	class parse_tree;
	class rule_node;
	class terminal_node;

	class parse_tree_visitor : public misc::visitor<parse_tree, terminal_node, error_node>
	{
	public:
		virtual void visit_children(rule_node const& node) = 0;
	};

	template<typename _TResult>
	class parse_tree_visitor_impl : public parse_tree_visitor
	{
	public:
		typedef _TResult result_type;
		typedef typename misc::param_type<_TResult>::type result_param_type;

	private:
		parse_tree_visitor_impl(parse_tree_visitor_impl const&) = delete;
		parse_tree_visitor_impl& operator= (parse_tree_visitor_impl const&) = delete;

	private:
		_TResult _result;

	public:
		explicit parse_tree_visitor_impl()
			: _result()
		{
		}

		// This will not have the expected result because it's not managed as a stack.
		//_TResult result() const
		//{
		//	return _result;
		//}

	public:
		virtual void visit(parse_tree const& node) override;
		virtual void visit(terminal_node const& node) override;
		virtual void visit(error_node const& node) override;
		virtual void visit_children(rule_node const& node) override;

	protected:
		virtual _TResult default_result();
		virtual _TResult aggregate_result(result_param_type aggregate, result_param_type next_result);
		virtual bool should_visit_next_child(rule_node const& node, result_param_type current_result);
	};

}
}
}

#include "parse_tree_visitor.inl"
