// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <antlr/test/test_graph_nodes.hpp>
#include <antlr/test/test_interval_set.hpp>
#include <antlr/test/test_visitor_inheritance.hpp>

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	antlr::test::test_graph_nodes();
	antlr::test::test_interval_set();
	antlr::test::test_visitor_inheritance();
	return 0;
}
