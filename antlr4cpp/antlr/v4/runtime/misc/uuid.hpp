// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <cstdint>
#include <functional>

namespace antlr4 {
namespace misc {

	class uuid
	{
	private:
		int32_t _a;
		int16_t _b;
		int16_t _c;
		uint8_t _d;
		uint8_t _e;
		uint8_t _f;
		uint8_t _g;
		uint8_t _h;
		uint8_t _i;
		uint8_t _j;
		uint8_t _k;

		friend struct std::hash<uuid>;
		friend bool operator== (uuid const& x, uuid const& y);

	public:
		uuid()
			: _a()
			, _b()
			, _c()
			, _d()
			, _e()
			, _f()
			, _g()
			, _h()
			, _i()
			, _j()
			, _k()
		{
		}

		uuid(int32_t a, int16_t b, int16_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h, uint8_t i, uint8_t j, uint8_t k)
			: _a(a)
			, _b(b)
			, _c(c)
			, _d(d)
			, _e(e)
			, _f(f)
			, _g(g)
			, _h(h)
			, _i(i)
			, _j(j)
			, _k(k)
		{
		}

		uuid(uint32_t a, uint16_t b, uint16_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h, uint8_t i, uint8_t j, uint8_t k)
			: _a(static_cast<int32_t>(a))
			, _b(static_cast<int16_t>(b))
			, _c(static_cast<int16_t>(c))
			, _d(d)
			, _e(e)
			, _f(f)
			, _g(g)
			, _h(h)
			, _i(i)
			, _j(j)
			, _k(k)
		{
		}
	};

	bool operator== (uuid const& x, uuid const& y)
	{
		if (x._a != y._a)
			return false;
		if (x._b != y._b)
			return false;
		if (x._c != y._c)
			return false;
		if (x._d != y._d)
			return false;
		if (x._e != y._e)
			return false;
		if (x._f != y._f)
			return false;
		if (x._g != y._g)
			return false;
		if (x._h != y._h)
			return false;
		if (x._i != y._i)
			return false;
		if (x._j != y._j)
			return false;
		if (x._k != y._k)
			return false;

		return true;
	}

}
}

namespace std {
	template<>
	struct hash<antlr4::misc::uuid>
	{
		size_t operator() (antlr4::misc::uuid const& value) const
		{
			auto result = value._a
				^ ((static_cast<int32_t>(value._b) << 16) | static_cast<int32_t>(static_cast<uint16_t>(value._c)))
				^ ((static_cast<int32_t>(value._f) << 24) | value._k);

			return static_cast<size_t>(result);
		}
	};
}
