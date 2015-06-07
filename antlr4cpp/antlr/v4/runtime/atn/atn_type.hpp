// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

namespace antlr4 {
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
