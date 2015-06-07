// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "prediction_context_cache.hpp"
#include "prediction_context.hpp"

namespace antlr4 {
namespace atn {

	inline bool operator== (prediction_context_cache::identity_commutative_prediction_context_operands const& x, prediction_context_cache::identity_commutative_prediction_context_operands const& y)
	{
		if (&x == &y)
			return true;

		return x.x() == y.x() && x.y() == y.y()
			|| x.x() == y.y() && x.y() == y.x();
	}

}
}

namespace std {

	inline size_t std::hash<antlr4::atn::prediction_context_cache::identity_commutative_prediction_context_operands>::operator() (antlr4::atn::prediction_context_cache::identity_commutative_prediction_context_operands const& x) const
	{
		std::hash<antlr4::atn::prediction_context> hasher;
		return hasher(*x.x()) ^ hasher(*x.y());
	}

}
