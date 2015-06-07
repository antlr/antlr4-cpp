// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <cassert>

#include "parse_tree_walker.hpp"

#include "parse_tree.hpp"
#include "parse_tree_listener.hpp"

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) )
#endif

namespace antlr4 {
namespace tree {

	namespace {

		template<class Target, class Source>
		inline Target polymorphic_downcast(Source* x)
		{
			assert(dynamic_cast<Target>(x) == static_cast<Target>(x));
			return static_cast<Target>(x);
		}

		template<class Target, class Source>
		inline Target polymorphic_downcast(Source& x)
		{
			assert(dynamic_cast<std::remove_reference<Target>::type*>(&x) == &static_cast<Target>(x));
			return static_cast<Target>(x);
		}

	}

	parse_tree_walker parse_tree_walker::_instance;

	void parse_tree_walker::walk(parse_tree_listener& listener, parse_tree const& tree)
	{
		switch (tree.type())
		{
		case parse_tree::tree_type::error:
		{
			error_node const& error = polymorphic_downcast<error_node const&>(tree);
			listener.visit(error);
			return;
		}

		case parse_tree::tree_type::terminal:
		{
			terminal_node const& terminal = polymorphic_downcast<terminal_node const&>(tree);
			listener.visit(terminal);
			return;
		}

		case parse_tree::tree_type::rule:
		{
			rule_node const& node = polymorphic_downcast<rule_node const&>(tree);
			enter_rule(listener, node);
			for (size_t i = 0; i < tree.size(); i++)
			{
				walk(listener, *tree.child(i));
			}

			exit_rule(listener, node);
			return;
		}

		default:
			assert(!"Invalid context type.");
			return;
		}
	}

	void parse_tree_walker::enter_rule(parse_tree_listener& listener, rule_node const& tree)
	{
		listener.enter_node(tree);
		tree.enter_rule(listener);
	}

	void parse_tree_walker::exit_rule(parse_tree_listener& listener, rule_node const& tree)
	{
		tree.exit_rule(listener);
		listener.exit_node(tree);
	}

}
}
