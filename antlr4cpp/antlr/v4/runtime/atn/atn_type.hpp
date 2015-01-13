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
