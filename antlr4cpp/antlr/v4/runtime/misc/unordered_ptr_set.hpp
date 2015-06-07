// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <unordered_set>

#include "ptr_equal_to.hpp"
#include "ptr_hash.hpp"

namespace antlr4 {
namespace misc {

	template<typename _Tptr, typename _Hasher = std::hash<std::remove_reference<decltype(*_Tptr())>::type>, typename _Alloc = std::allocator<_Tptr>>
	using unordered_ptr_set = std::unordered_set<_Tptr, ptr_hash<_Tptr, _Hasher>, ptr_equal_to<_Tptr>>;

}
}
