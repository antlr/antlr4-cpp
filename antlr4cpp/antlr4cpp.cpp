// antlr4cpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <antlr/test/test_graph_nodes.hpp>
#include <antlr/test/test_interval_set.hpp>

int _tmain(int /*argc*/, _TCHAR* /*argv*/[])
{
	antlr::test::test_graph_nodes();
	antlr::test::test_interval_set();
	return 0;
}
