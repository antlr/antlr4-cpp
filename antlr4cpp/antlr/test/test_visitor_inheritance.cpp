// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <iostream>

#include "test_visitor_inheritance.hpp"

#include <antlr/v4/runtime/tree/parse_tree.hpp>
#include <antlr/v4/runtime/tree/parse_tree_visitor.hpp>

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) )
#endif

namespace antlr {
namespace test {

	using namespace antlr4;
	using namespace antlr4::misc;
	using namespace antlr4::tree;

	namespace {

		class sample_rule_node : public rule_node, public visitable<sample_rule_node>
		{
		public:
			explicit sample_rule_node()
				: rule_node(nullptr)
			{
			}

		public:
			virtual std::shared_ptr<parse_tree> parent() const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual std::shared_ptr<parse_tree> child(size_t /*index*/) const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual size_t size() const override
			{
				return 0;
				//throw std::runtime_error("not implemented");
			}

			virtual std::pair<size_t, size_t> source_interval() const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual std::wstring text() const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual std::wstring tree_text() const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual void accept(parse_tree_visitor& visitor) const override
			{
				misc::visitor<sample_rule_node>* typed_visitor = dynamic_cast<misc::visitor<sample_rule_node>*>(&visitor);
				if (typed_visitor)
				{
					typed_visitor->visit(*this);
				}
				else
				{
					visitor.visit_children(*this);
				}
			}

			virtual void accept(visitor<sample_rule_node>& visitor) const override
			{
				visitor.visit(*this);
			}
		};

		class sample_rule_node2 : public rule_node, public visitable<sample_rule_node2>
		{
		public:
			explicit sample_rule_node2()
				: rule_node(nullptr)
			{
			}

		public:
			virtual std::shared_ptr<parse_tree> parent() const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual std::shared_ptr<parse_tree> child(size_t /*index*/) const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual size_t size() const override
			{
				return 0;
				//throw std::runtime_error("not implemented");
			}

			virtual std::pair<size_t, size_t> source_interval() const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual std::wstring text() const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual std::wstring tree_text() const override
			{
				throw std::runtime_error("not implemented");
			}

			virtual void accept(parse_tree_visitor& visitor) const override
			{
				misc::visitor<sample_rule_node2>* typed_visitor = dynamic_cast<misc::visitor<sample_rule_node2>*>(&visitor);
				if (typed_visitor)
				{
					typed_visitor->visit(*this);
				}
				else
				{
					visitor.visit_children(*this);
				}
			}

			virtual void accept(visitor<sample_rule_node2>& visitor) const override
			{
				visitor.visit(*this);
			}
		};

		class sample_parser_visitor : public visitor<sample_rule_node, sample_rule_node2>, public parse_tree_visitor
		{
		};

		template<typename _TResult>
		class sample_parser_visitor_impl : public parse_tree_visitor_impl<_TResult>, public sample_parser_visitor
		{
		public:
			virtual void visit(parse_tree const& node) override
			{
				std::wcout << L"visiting unexpected call" << std::endl;
				parse_tree_visitor_impl<_TResult>::visit(node);
			};

			virtual void visit(terminal_node const& node) override
			{
				parse_tree_visitor_impl<_TResult>::visit(node);
			};

			virtual void visit(error_node const& node) override
			{
				parse_tree_visitor_impl<_TResult>::visit(node);
			};

			virtual void visit_children(rule_node const& node) override
			{
				parse_tree_visitor_impl<_TResult>::visit_children(node);
			};

			virtual void visit(sample_rule_node const& node) override
			{
				std::wcout << L"visiting sample rule node" << std::endl;
				visit_children(node);
			}

			virtual void visit(sample_rule_node2 const& node) override
			{
				std::wcout << L"visiting sample rule node 2" << std::endl;
				visit_children(node);
			}
		};

		class general_parse_tree_visitor : public parse_tree_visitor_impl<int>
		{
		public:
			virtual void visit(parse_tree const& node) override
			{
				std::wcout << L"visiting basic node" << std::endl;
				parse_tree_visitor_impl::visit(node);
			}
		};

	}

	void test_visitor_inheritance()
	{
		sample_rule_node ptr;
		sample_rule_node2 ptr2;

		sample_parser_visitor_impl<int> visitor1;
		visitor<sample_rule_node>& typed_visitor = visitor1;
		visitor<sample_rule_node2>& typed_visitor1 = visitor1;
		visitor1.visit(ptr);
		visitor1.visit(ptr2);
		typed_visitor.visit(ptr);
		typed_visitor1.visit(ptr2);
		ptr.accept(typed_visitor);
		ptr2.accept(typed_visitor1);

		general_parse_tree_visitor visitor2;
		visitor2.visit(ptr);
	}

}
}
