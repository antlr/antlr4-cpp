/*
 *  Copyright (c) 2015 Sam Harwell, Tunnel Vision Laboratories LLC
 *  All rights reserved.
 *
 *  The source code of this document is proprietary work, and is not licensed for
 *  distribution. For information about licensing, contact Sam Harwell at:
 *      sam@tunnelvisionlabs.com
 */
#pragma once

#include <functional>

namespace antlr4 {
namespace runtime {
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
}
