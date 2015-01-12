#pragma once

#include <functional>

namespace antlr4 {
namespace runtime {
namespace misc {

	template<typename _Tptr>
	struct ptr_equal_to : public std::binary_function<_Tptr, _Tptr, bool>
	{
		typedef typename std::conditional<std::is_pointer<_Tptr>::value, _Tptr, _Tptr const&>::type param_type;

		bool operator() (param_type _Left, param_type _Right) const
		{
			if (!_Left)
			{
				return !_Right;
			}
			else if (!_Right)
			{
				return false;
			}

			return *_Left == *_Right;
		}
	};

}
}
}
