// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <antlr/test/test_graph_nodes.hpp>
#include <antlr/v4/runtime/atn/prediction_context.hpp>
#include <antlr/v4/runtime/atn/prediction_context_cache.hpp>

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) )
#endif

namespace antlr {
namespace test {

	using namespace antlr4::atn;

	namespace {

		// ------------ SUPPORT -------------------------

		prediction_context_cache context_cache;

		std::shared_ptr<prediction_context> a(bool fullContext);
		std::shared_ptr<prediction_context> b(bool fullContext);
		std::shared_ptr<prediction_context> c(bool fullContext);
		std::shared_ptr<prediction_context> d(bool fullContext);
		std::shared_ptr<prediction_context> u(bool fullContext);
		std::shared_ptr<prediction_context> v(bool fullContext);
		std::shared_ptr<prediction_context> w(bool fullContext);
		std::shared_ptr<prediction_context> x(bool fullContext);
		std::shared_ptr<prediction_context> y(bool fullContext);
		std::shared_ptr<prediction_context> create_singleton(std::shared_ptr<prediction_context> const& parent, int return_state);
		std::shared_ptr<prediction_context> array(std::shared_ptr<prediction_context> const& context);
		std::shared_ptr<prediction_context> array(std::shared_ptr<prediction_context> const& context0, std::shared_ptr<prediction_context> const& context1);
		std::wstring to_dot_string(std::shared_ptr<prediction_context> const& context);

		// ------------ TESTS -------------------------

		void test_root_root()
		{
			std::shared_ptr<prediction_context> r(context_cache.join(prediction_context::empty_local, prediction_context::empty_local));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"*\"];\n"
				L"}\n";

			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_root_root_fullctx()
		{
			std::shared_ptr<prediction_context> r(context_cache.join(prediction_context::empty_full, prediction_context::empty_full));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"$\"];\n"
				L"}\n";

			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_x_root()
		{
			std::shared_ptr<prediction_context> r(context_cache.join(x(false), prediction_context::empty_local));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"*\"];\n"
				L"}\n";

			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_x_root_fullctx()
		{
			std::shared_ptr<prediction_context> r(context_cache.join(x(true), prediction_context::empty_full));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>$\"];\n"
				L"  s1[label=\"$\"];\n"
				L"  s0:p0->s1[label=\"9\"];\n"
				L"}\n";

			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_root_x()
		{
			std::shared_ptr<prediction_context> r(context_cache.join(prediction_context::empty_local, x(false)));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"*\"];\n"
				L"}\n";

			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_root_x_fullctx()
		{
			std::shared_ptr<prediction_context> r = context_cache.join(prediction_context::empty_full, x(true));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>$\"];\n"
				L"  s1[label=\"$\"];\n"
				L"  s0:p0->s1[label=\"9\"];\n"
				L"}\n";

			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_a_a()
		{
			std::shared_ptr<prediction_context> r = context_cache.join(a(false), a(false));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_aroot_ax()
		{
			std::shared_ptr<prediction_context> a1(a(false));
			std::shared_ptr<prediction_context> x(x(false));
			std::shared_ptr<prediction_context> a2(create_singleton(x, 1));
			std::shared_ptr<prediction_context> r(context_cache.join(a1, a2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_aroot_ax_fullctx()
		{
			std::shared_ptr<prediction_context> a1(a(true));
			std::shared_ptr<prediction_context> x(x(true));
			std::shared_ptr<prediction_context> a2(create_singleton(x, 1));
			std::shared_ptr<prediction_context> r(context_cache.join(a1, a2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[shape=record, label=\"<p0>|<p1>$\"];\n"
				L"  s2[label=\"$\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"  s1:p0->s2[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_axroot_aroot()
		{
			std::shared_ptr<prediction_context> x(x(false));
			std::shared_ptr<prediction_context> a1(create_singleton(x, 1));
			std::shared_ptr<prediction_context> a2(a(false));
			std::shared_ptr<prediction_context> r(context_cache.join(a1, a2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_aaroot_aroot_root_fullCtx()
		{
			std::shared_ptr<prediction_context> empty(prediction_context::empty_full);
			std::shared_ptr<prediction_context> child1(create_singleton(empty, 8));
			std::shared_ptr<prediction_context> right(context_cache.join(empty, child1));
			std::shared_ptr<prediction_context> left(create_singleton(right, 8));
			std::shared_ptr<prediction_context> merged(context_cache.join(left, right));
			std::wstring actual(to_dot_string(merged));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>$\"];\n"
				L"  s1[shape=record, label=\"<p0>|<p1>$\"];\n"
				L"  s2[label=\"$\"];\n"
				L"  s0:p0->s1[label=\"8\"];\n"
				L"  s1:p0->s2[label=\"8\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_axroot_aroot_fullctx()
		{
			std::shared_ptr<prediction_context> x(x(true));
			std::shared_ptr<prediction_context> a1(create_singleton(x, 1));
			std::shared_ptr<prediction_context> a2(a(true));
			std::shared_ptr<prediction_context> r(context_cache.join(a1, a2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[shape=record, label=\"<p0>|<p1>$\"];\n"
				L"  s2[label=\"$\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"  s1:p0->s2[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_a_b()
		{
			std::shared_ptr<prediction_context> r(context_cache.join(a(false), b(false)));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_ax_ax_same()
		{
			std::shared_ptr<prediction_context> x(x(false));
			std::shared_ptr<prediction_context> a1(create_singleton(x, 1));
			std::shared_ptr<prediction_context> a2(create_singleton(x, 1));
			std::shared_ptr<prediction_context> r(context_cache.join(a1, a2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s2[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"  s1->s2[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_ax_ax()
		{
			std::shared_ptr<prediction_context> x1(x(false));
			std::shared_ptr<prediction_context> x2(x(false));
			std::shared_ptr<prediction_context> a1(create_singleton(x1, 1));
			std::shared_ptr<prediction_context> a2(create_singleton(x2, 1));
			std::shared_ptr<prediction_context> r(context_cache.join(a1, a2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s2[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"  s1->s2[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_abx_abx()
		{
			std::shared_ptr<prediction_context> x1(x(false));
			std::shared_ptr<prediction_context> x2(x(false));
			std::shared_ptr<prediction_context> b1(create_singleton(x1, 2));
			std::shared_ptr<prediction_context> b2(create_singleton(x2, 2));
			std::shared_ptr<prediction_context> a1(create_singleton(b1, 1));
			std::shared_ptr<prediction_context> a2(create_singleton(b2, 1));
			std::shared_ptr<prediction_context> r(context_cache.join(a1, a2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s3[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"  s1->s2[label=\"2\"];\n"
				L"  s2->s3[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_abx_acx()
		{
			std::shared_ptr<prediction_context> x1(x(false));
			std::shared_ptr<prediction_context> x2(x(false));
			std::shared_ptr<prediction_context> b(create_singleton(x1, 2));
			std::shared_ptr<prediction_context> c(create_singleton(x2, 3));
			std::shared_ptr<prediction_context> a1(create_singleton(b, 1));
			std::shared_ptr<prediction_context> a2(create_singleton(c, 1));
			std::shared_ptr<prediction_context> r(context_cache.join(a1, a2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s3[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"  s1:p0->s2[label=\"2\"];\n"
				L"  s1:p1->s2[label=\"3\"];\n"
				L"  s2->s3[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_ax_bx_same()
		{
			std::shared_ptr<prediction_context> x(x(false));
			std::shared_ptr<prediction_context> a(create_singleton(x, 1));
			std::shared_ptr<prediction_context> b(create_singleton(x, 2));
			std::shared_ptr<prediction_context> r(context_cache.join(a, b));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s2[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"  s1->s2[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_ax_bx()
		{
			std::shared_ptr<prediction_context> x1(x(false));
			std::shared_ptr<prediction_context> x2(x(false));
			std::shared_ptr<prediction_context> a(create_singleton(x1, 1));
			std::shared_ptr<prediction_context> b(create_singleton(x2, 2));
			std::shared_ptr<prediction_context> r(context_cache.join(a, b));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s2[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"  s1->s2[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_ax_by()
		{
			std::shared_ptr<prediction_context> a(create_singleton(x(false), 1));
			std::shared_ptr<prediction_context> b(create_singleton(y(false), 2));
			std::shared_ptr<prediction_context> r(context_cache.join(a, b));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s3[label=\"*\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s2->s3[label=\"10\"];\n"
				L"  s1->s3[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_aroot_bx()
		{
			std::shared_ptr<prediction_context> x2(x(false));
			std::shared_ptr<prediction_context> a(a(false));
			std::shared_ptr<prediction_context> b(create_singleton(x2, 2));
			std::shared_ptr<prediction_context> r(context_cache.join(a, b));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s2->s1[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_aroot_bx_fullctx()
		{
			std::shared_ptr<prediction_context> x2(x(true));
			std::shared_ptr<prediction_context> a(a(true));
			std::shared_ptr<prediction_context> b(create_singleton(x2, 2));
			std::shared_ptr<prediction_context> r(context_cache.join(a, b));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s1[label=\"$\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s2->s1[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_aex_bfx()
		{
			std::shared_ptr<prediction_context> x1(x(false));
			std::shared_ptr<prediction_context> x2(x(false));
			std::shared_ptr<prediction_context> e(create_singleton(x1, 5));
			std::shared_ptr<prediction_context> f(create_singleton(x2, 6));
			std::shared_ptr<prediction_context> a(create_singleton(e, 1));
			std::shared_ptr<prediction_context> b(create_singleton(f, 2));
			std::shared_ptr<prediction_context> r(context_cache.join(a, b));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s3[label=\"3\"];\n"
				L"  s4[label=\"*\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s2->s3[label=\"6\"];\n"
				L"  s3->s4[label=\"9\"];\n"
				L"  s1->s3[label=\"5\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		// Array merges

		void test_Aroot_Aroot_fullctx()
		{
			std::shared_ptr<prediction_context> A1(array(prediction_context::empty_full));
			std::shared_ptr<prediction_context> A2(array(prediction_context::empty_full));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"$\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aab_Ac()
		{
			// a,b + c
			std::shared_ptr<prediction_context> a(a(false));
			std::shared_ptr<prediction_context> b(b(false));
			std::shared_ptr<prediction_context> c(c(false));
			std::shared_ptr<prediction_context> A1(array(a, b));
			std::shared_ptr<prediction_context> A2(array(c));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"  s0:p2->s1[label=\"3\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aa_Aa()
		{
			std::shared_ptr<prediction_context> a1(a(false));
			std::shared_ptr<prediction_context> a2(a(false));
			std::shared_ptr<prediction_context> A1(array(a1));
			std::shared_ptr<prediction_context> A2(array(a2));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aa_Abc()
		{
			// a + b,c
			std::shared_ptr<prediction_context> a(a(false));
			std::shared_ptr<prediction_context> b(b(false));
			std::shared_ptr<prediction_context> c(c(false));
			std::shared_ptr<prediction_context> A1(array(a));
			std::shared_ptr<prediction_context> A2(array(b, c));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"  s0:p2->s1[label=\"3\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aac_Ab()
		{
			// a,c + b
			std::shared_ptr<prediction_context> a(a(false));
			std::shared_ptr<prediction_context> b(b(false));
			std::shared_ptr<prediction_context> c(c(false));
			std::shared_ptr<prediction_context> A1(array(a, c));
			std::shared_ptr<prediction_context> A2(array(b));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"  s0:p2->s1[label=\"3\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aab_Aa()
		{
			// a,b + a
			std::shared_ptr<prediction_context> A1(array(a(false), b(false)));
			std::shared_ptr<prediction_context> A2(array(a(false)));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aab_Ab()
		{
			// a,b + b
			std::shared_ptr<prediction_context> A1(array(a(false), b(false)));
			std::shared_ptr<prediction_context> A2(array(b(false)));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s1[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aax_Aby()
		{
			// ax + by but in arrays
			std::shared_ptr<prediction_context> a(create_singleton(x(false), 1));
			std::shared_ptr<prediction_context> b(create_singleton(y(false), 2));
			std::shared_ptr<prediction_context> A1(array(a));
			std::shared_ptr<prediction_context> A2(array(b));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s3[label=\"*\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s2->s3[label=\"10\"];\n"
				L"  s1->s3[label=\"9\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aax_Aay()
		{
			// ax + ay -> merged singleton a, array parent
			std::shared_ptr<prediction_context> a1(create_singleton(x(false), 1));
			std::shared_ptr<prediction_context> a2(create_singleton(y(false), 1));
			std::shared_ptr<prediction_context> A1(array(a1));
			std::shared_ptr<prediction_context> A2(array(a2));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[label=\"0\"];\n"
				L"  s1[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s2[label=\"*\"];\n"
				L"  s0->s1[label=\"1\"];\n"
				L"  s1:p0->s2[label=\"9\"];\n"
				L"  s1:p1->s2[label=\"10\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aaxc_Aayd()
		{
			// ax,c + ay,d -> merged a, array parent
			std::shared_ptr<prediction_context> a1(create_singleton(x(false), 1));
			std::shared_ptr<prediction_context> a2(create_singleton(y(false), 1));
			std::shared_ptr<prediction_context> A1(array(a1, c(false)));
			std::shared_ptr<prediction_context> A2(array(a2, d(false)));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>\"];\n"
				L"  s2[label=\"*\"];\n"
				L"  s1[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"3\"];\n"
				L"  s0:p2->s2[label=\"4\"];\n"
				L"  s1:p0->s2[label=\"9\"];\n"
				L"  s1:p1->s2[label=\"10\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aaubv_Acwdx()
		{
			// au,bv + cw,dx -> [a,b,c,d]->[u,v,w,x]
			std::shared_ptr<prediction_context> a(create_singleton(u(false), 1));
			std::shared_ptr<prediction_context> b(create_singleton(v(false), 2));
			std::shared_ptr<prediction_context> c(create_singleton(w(false), 3));
			std::shared_ptr<prediction_context> d(create_singleton(x(false), 4));
			std::shared_ptr<prediction_context> A1(array(a, b));
			std::shared_ptr<prediction_context> A2(array(c, d));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>|<p3>\"];\n"
				L"  s4[label=\"4\"];\n"
				L"  s5[label=\"*\"];\n"
				L"  s3[label=\"3\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s0:p2->s3[label=\"3\"];\n"
				L"  s0:p3->s4[label=\"4\"];\n"
				L"  s4->s5[label=\"9\"];\n"
				L"  s3->s5[label=\"8\"];\n"
				L"  s2->s5[label=\"7\"];\n"
				L"  s1->s5[label=\"6\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aaubv_Abvdx()
		{
			// au,bv + bv,dx -> [a,b,d]->[u,v,x]
			std::shared_ptr<prediction_context> a(create_singleton(u(false), 1));
			std::shared_ptr<prediction_context> b1(create_singleton(v(false), 2));
			std::shared_ptr<prediction_context> b2(create_singleton(v(false), 2));
			std::shared_ptr<prediction_context> d(create_singleton(x(false), 4));
			std::shared_ptr<prediction_context> A1(array(a, b1));
			std::shared_ptr<prediction_context> A2(array(b2, d));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>\"];\n"
				L"  s3[label=\"3\"];\n"
				L"  s4[label=\"*\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s0:p2->s3[label=\"4\"];\n"
				L"  s3->s4[label=\"9\"];\n"
				L"  s2->s4[label=\"7\"];\n"
				L"  s1->s4[label=\"6\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aaubv_Abwdx()
		{
			// au,bv + bw,dx -> [a,b,d]->[u,[v,w],x]
			std::shared_ptr<prediction_context> a(create_singleton(u(false), 1));
			std::shared_ptr<prediction_context> b1(create_singleton(v(false), 2));
			std::shared_ptr<prediction_context> b2(create_singleton(w(false), 2));
			std::shared_ptr<prediction_context> d(create_singleton(x(false), 4));
			std::shared_ptr<prediction_context> A1(array(a, b1));
			std::shared_ptr<prediction_context> A2(array(b2, d));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>\"];\n"
				L"  s3[label=\"3\"];\n"
				L"  s4[label=\"*\"];\n"
				L"  s2[shape=record, label=\"<p0>|<p1>\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s0:p2->s3[label=\"4\"];\n"
				L"  s3->s4[label=\"9\"];\n"
				L"  s2:p0->s4[label=\"7\"];\n"
				L"  s2:p1->s4[label=\"8\"];\n"
				L"  s1->s4[label=\"6\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aaubv_Abvdu()
		{
			// au,bv + bv,du -> [a,b,d]->[u,v,u]; u,v shared
			std::shared_ptr<prediction_context> a(create_singleton(u(false), 1));
			std::shared_ptr<prediction_context> b1(create_singleton(v(false), 2));
			std::shared_ptr<prediction_context> b2(create_singleton(v(false), 2));
			std::shared_ptr<prediction_context> d(create_singleton(u(false), 4));
			std::shared_ptr<prediction_context> A1(array(a, b1));
			std::shared_ptr<prediction_context> A2(array(b2, d));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>\"];\n"
				L"  s2[label=\"2\"];\n"
				L"  s3[label=\"*\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s2[label=\"2\"];\n"
				L"  s0:p2->s1[label=\"4\"];\n"
				L"  s2->s3[label=\"7\"];\n"
				L"  s1->s3[label=\"6\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		void test_Aaubu_Acudu()
		{
			// au,bu + cu,du -> [a,b,c,d]->[u,u,u,u]
			std::shared_ptr<prediction_context> a(create_singleton(u(false), 1));
			std::shared_ptr<prediction_context> b(create_singleton(u(false), 2));
			std::shared_ptr<prediction_context> c(create_singleton(u(false), 3));
			std::shared_ptr<prediction_context> d(create_singleton(u(false), 4));
			std::shared_ptr<prediction_context> A1(array(a, b));
			std::shared_ptr<prediction_context> A2(array(c, d));
			std::shared_ptr<prediction_context> r(context_cache.join(A1, A2));
			std::wstring actual(to_dot_string(r));
			std::wstring expecting =
				L"digraph G {\n"
				L"rankdir=LR;\n"
				L"  s0[shape=record, label=\"<p0>|<p1>|<p2>|<p3>\"];\n"
				L"  s1[label=\"1\"];\n"
				L"  s2[label=\"*\"];\n"
				L"  s0:p0->s1[label=\"1\"];\n"
				L"  s0:p1->s1[label=\"2\"];\n"
				L"  s0:p2->s1[label=\"3\"];\n"
				L"  s0:p3->s1[label=\"4\"];\n"
				L"  s1->s2[label=\"6\"];\n"
				L"}\n";
		
			std::wcout << actual << std::endl;
			assert(actual == expecting);
		}

		// ------------ SUPPORT -------------------------

		std::shared_ptr<prediction_context> a(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 1);
		}

		std::shared_ptr<prediction_context> b(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 2);
		}

		std::shared_ptr<prediction_context> c(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 3);
		}

		std::shared_ptr<prediction_context> d(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 4);
		}

		std::shared_ptr<prediction_context> u(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 6);
		}

		std::shared_ptr<prediction_context> v(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 7);
		}

		std::shared_ptr<prediction_context> w(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 8);
		}

		std::shared_ptr<prediction_context> x(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 9);
		}

		std::shared_ptr<prediction_context> y(bool fullContext) {
			return create_singleton(fullContext ? prediction_context::empty_full : prediction_context::empty_local, 10);
		}

		std::shared_ptr<prediction_context> create_singleton(std::shared_ptr<prediction_context> const& parent, int return_state) {
			return context_cache.get_child(parent, return_state);
		}

		std::shared_ptr<prediction_context> array(std::shared_ptr<prediction_context> const& context) {
			return context;
		}

		std::shared_ptr<prediction_context> array(std::shared_ptr<prediction_context> const& context0, std::shared_ptr<prediction_context> const& context1) {
			std::shared_ptr<prediction_context> result = context0;
			result = context_cache.join(result, context1);
			return result;
		}

		// The default hash function for pointers is reference equality. Since C++ does not move objects in memory, we
		// don't need to provide the specialized hash mechanism here to avoid the computation of an identity hash code
		// like the Java code needs.
		typedef std::unordered_map<std::shared_ptr<prediction_context>, std::shared_ptr<prediction_context>> identity_map;
		typedef std::unordered_map<std::shared_ptr<prediction_context>, size_t> id_map;

		std::wstring to_dot_string(std::shared_ptr<prediction_context> const& context)
		{
			std::wostringstream nodes;
			std::wostringstream edges;

			identity_map visited;
			id_map context_ids;

			std::deque<std::shared_ptr<prediction_context>> work_list;
			visited.insert(std::make_pair(context, context));
			context_ids.insert(std::make_pair(context, context_ids.size()));
			work_list.push_back(context);
			while (!work_list.empty())
			{
				std::shared_ptr<prediction_context> current(std::move(work_list.back()));
				work_list.pop_back();
				nodes << L"  s";
				nodes << context_ids[current];
				nodes << L"[";

				if (current->size() > 1) {
					nodes << L"shape=record, ";
				}

				nodes << L"label=\"";

				if (current->is_empty())
				{
					nodes << (current->is_empty_local() ? L"*" : L"$");
				}
				else if (current->size() > 1)
				{
					for (size_t i = 0; i < current->size(); i++)
					{
						if (i > 0)
						{
							nodes << L"|";
						}

						nodes << L"<p" << i << L">";
						if (current->return_state(i) == prediction_context::empty_full_state_key)
						{
							nodes << L"$";
						}
						else if (current->return_state(i) == prediction_context::empty_local_state_key)
						{
							nodes << L"*";
						}
					}
				}
				else
				{
					nodes << context_ids[current];
				}

				nodes << L"\"];\n";

				for (size_t i = 0; i < current->size(); i++)
				{
					if (current->return_state(i) == prediction_context::empty_full_state_key
						|| current->return_state(i) == prediction_context::empty_local_state_key)
					{
						continue;
					}

					if (visited.insert(std::make_pair(current->parent(i), current->parent(i))).second)
					{
						context_ids.insert(std::make_pair(current->parent(i), context_ids.size()));
						work_list.push_back(current->parent(i));
					}

					edges << L"  s" << context_ids[current];
					if (current->size() > 1)
					{
						edges << L":p" << i;
					}

					edges << L"->";
					edges << L"s" << context_ids[current->parent(i)];
					edges << L"[label=\"" << current->return_state(i) << L"\"]";
					edges << L";\n";
				}
			}

			std::wstring result;
			result.append(L"digraph G {\n");
			result.append(L"rankdir=LR;\n");
			result.append(nodes.str());
			result.append(edges.str());
			result.append(L"}\n");
			return std::move(result);
		}
	}

	void test_graph_nodes()
	{
		test_root_root();
		test_root_root_fullctx();
		test_x_root();
		test_x_root_fullctx();
		test_root_x();
		test_root_x_fullctx();
		test_a_a();
		test_aroot_ax();
		test_aroot_ax_fullctx();
		test_axroot_aroot();
		test_aaroot_aroot_root_fullCtx();
		test_axroot_aroot_fullctx();
		test_a_b();
		test_ax_ax_same();
		test_ax_ax();
		test_abx_abx();
		test_abx_acx();
		test_ax_bx_same();
		test_ax_bx();
		test_ax_by();
		test_aroot_bx();
		test_aroot_bx_fullctx();
		test_aex_bfx();
		test_Aroot_Aroot_fullctx();
		test_Aab_Ac();
		test_Aa_Aa();
		test_Aa_Abc();
		test_Aac_Ab();
		test_Aab_Aa();
		test_Aab_Ab();
		test_Aax_Aby();
		test_Aax_Aay();
		test_Aaxc_Aayd();
		test_Aaubv_Acwdx();
		test_Aaubv_Abvdx();
		test_Aaubv_Abwdx();
		test_Aaubv_Abvdu();
		test_Aaubu_Acudu();
	}

}
}
