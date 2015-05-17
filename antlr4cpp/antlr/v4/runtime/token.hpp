/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
#pragma once

#include <cstdint>

namespace antlr4 {
namespace runtime {

	class token
	{
	public:
		static const int32_t epsilon = -2;
		static const int32_t eof = -1;
	};

}
}
