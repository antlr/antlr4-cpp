// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>

namespace antlr4 {
namespace atn {

	class prediction_context;

	class prediction_context_cache
	{
		class data;

		prediction_context_cache(prediction_context_cache const&) = delete;
		prediction_context_cache& operator= (prediction_context_cache const&) = delete;

	private:
		std::unique_ptr<data> private_data;

	public:
		prediction_context_cache(bool enable_cache = true);
		prediction_context_cache(prediction_context_cache&& cache);
		~prediction_context_cache();

	public:
		static prediction_context_cache uncached();

	public:
		std::shared_ptr<prediction_context> get_as_cached(std::shared_ptr<prediction_context> const& context);
		std::shared_ptr<prediction_context> get_child(std::shared_ptr<prediction_context> const& context, int return_state);
		std::shared_ptr<prediction_context> join(std::shared_ptr<prediction_context> const& x, std::shared_ptr<prediction_context> const& y);

	public:
		class identity_commutative_prediction_context_operands
		{
		private:
			const std::shared_ptr<prediction_context> _x;
			const std::shared_ptr<prediction_context> _y;

		public:
			identity_commutative_prediction_context_operands(std::shared_ptr<prediction_context> && x, std::shared_ptr<prediction_context> && y)
				: _x(x)
				, _y(y)
			{
			}

			std::shared_ptr<prediction_context> const& x() const
			{
				return _x;
			}

			std::shared_ptr<prediction_context> const& y() const
			{
				return _y;
			}
		};
	};

	inline bool operator== (prediction_context_cache::identity_commutative_prediction_context_operands const& x, prediction_context_cache::identity_commutative_prediction_context_operands const& y);

}
}

namespace std {

	template<>
	struct hash<antlr4::atn::prediction_context_cache::identity_commutative_prediction_context_operands>
	{
		inline size_t operator()(antlr4::atn::prediction_context_cache::identity_commutative_prediction_context_operands const& x) const;
	};

}

#include "prediction_context_cache.inl"
