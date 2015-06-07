// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <functional>

namespace antlr4 {
namespace misc {

	template<typename _Tptr, typename _Hasher = std::hash<std::remove_reference<decltype(*_Tptr())>::type>>
	struct ptr_hash : public std::unary_function<_Tptr, size_t>
	{
		typedef typename param_type<_Tptr>::type param_type;

		size_t operator() (param_type value) const
		{
			if (!value)
			{
				return std::hash<void*>()(nullptr);
			}

			return _Hasher()(*value);
		}
	};

}
}
