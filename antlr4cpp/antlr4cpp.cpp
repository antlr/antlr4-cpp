/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
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
