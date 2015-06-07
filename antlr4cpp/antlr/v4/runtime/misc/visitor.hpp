// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

namespace antlr4 {
namespace misc {

	template<typename... _Types>
	class visitor;

	template<typename _T>
	class visitor<_T>
	{
	protected:
		virtual ~visitor() { }

	public:
		virtual void visit(_T const& node) = 0;
	};

	template<typename _T, typename... _Types>
	class visitor<_T, _Types...> : public visitor<_T>, public visitor<_Types...>
	{
	public:
		using visitor<_Types...>::visit;
		using visitor<_T>::visit;
	};

	template<typename... _Types>
	class visitable
	{
	protected:
		virtual ~visitable() { }

	public:
		virtual void accept(visitor<_Types...>& visitor) const = 0;
	};

}
}
