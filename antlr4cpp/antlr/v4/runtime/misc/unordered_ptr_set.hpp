/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
#pragma once

#include <unordered_set>

#include "ptr_equal_to.hpp"
#include "ptr_hash.hpp"

namespace antlr4 {
namespace runtime {
namespace misc {

	template<typename _Tptr, typename _Hasher = std::hash<std::remove_reference<decltype(*_Tptr())>::type>, typename _Alloc = std::allocator<_Tptr>>
	using unordered_ptr_set = std::unordered_set<_Tptr, ptr_hash<_Tptr, _Hasher>, ptr_equal_to<_Tptr>>;

}
}
}
