/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
#include "prediction_context_cache.hpp"
#include "prediction_context.hpp"

namespace antlr4 {
namespace runtime {
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
}

namespace std {

	inline size_t std::hash<antlr4::runtime::atn::prediction_context_cache::identity_commutative_prediction_context_operands>::operator() (antlr4::runtime::atn::prediction_context_cache::identity_commutative_prediction_context_operands const& x) const
	{
		std::hash<antlr4::runtime::atn::prediction_context> hasher;
		return hasher(*x.x()) ^ hasher(*x.y());
	}

}
