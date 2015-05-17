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
namespace atn {

	// Represents the type of recognizer an ATN applies to.
	enum class atn_type
	{
		// A lexer grammar
		lexer,

		// A parser grammar
		parser,
	};

}
}
}
