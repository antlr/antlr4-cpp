// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <cstdint>
#include <functional>

namespace antlr4 {
namespace misc {

	struct murmur_hash
	{
	private:
		murmur_hash() = delete;
		murmur_hash(murmur_hash const&) = delete;
		murmur_hash& operator= (murmur_hash const&) = delete;

	private:
		static const int32_t default_seed = 0;

	public:
		static int32_t initialize(int32_t seed = default_seed)
		{
			return seed;
		}

		static int32_t update(int32_t hash, int32_t value)
		{
			const int32_t c1 = 0xCC9E2D51;
			const int32_t c2 = 0x1B873593;
			const int32_t r1 = 15;
			const int32_t r2 = 13;
			const int32_t m = 5;
			const int32_t n = 0xE6546B64;

			int32_t k = value;
			k = k * c1;
			k = (k << r1) | static_cast<int32_t>(static_cast<uint32_t>(k) >> (32 - r1));
			k = k * c2;

			hash = hash ^ k;
			hash = (hash << r2) | static_cast<int32_t>(static_cast<uint32_t>(hash) >> (32 - r2));
			hash = hash * m + n;

			return hash;
		}

		template<typename _T, typename _Hasher = std::hash<_T>>
		static int32_t update(int32_t hash, _T value, _Hasher const& hasher)
		{
			return update(hash, static_cast<int32_t>(hasher(value)));
		}

		template<typename _T, typename _Hasher = std::hash<_T>>
		static int32_t update(int32_t hash, _T value)
		{
			return update(hash, value, _Hasher());
		}

		static int32_t finish(int32_t hash, size_t number_of_words)
		{
			hash = hash ^ static_cast<int32_t>(number_of_words * 4);
			hash = hash ^ static_cast<int32_t>(static_cast<uint32_t>(hash) >> 16);
			hash = hash * static_cast<int32_t>(0x85EBCA6B);
			hash = hash ^ static_cast<int32_t>(static_cast<uint32_t>(hash) >> 13);
			hash = hash * static_cast<int32_t>(0xC2B2AE35);
			hash = hash ^ static_cast<int32_t>(static_cast<uint32_t>(hash) >> 15);
			return hash;
		}
	};

}
}
