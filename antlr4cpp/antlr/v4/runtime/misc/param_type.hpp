#pragma once

#include <functional>

namespace antlr4 {
namespace runtime {
namespace misc {

	template<typename _Ty>
	struct param_type
	{
	private:
		static const bool pass_by_value = std::is_pointer<_Ty>::value || std::is_arithmetic<_Ty>::value;

	public:
		typedef typename std::conditional<pass_by_value, _Ty, _Ty const&>::type type;
	};

}
}
}
