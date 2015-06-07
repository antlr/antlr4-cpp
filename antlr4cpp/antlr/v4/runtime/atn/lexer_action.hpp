// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>

namespace antlr4 {
namespace atn {

	class lexer_action
	{
	public:
		enum class lexer_action_type
		{
			//
			// position independent
			//
			channel,
			mode,
			more,
			pop_mode,
			push_mode,
			skip,
			type,

			//
			// position dependent
			//
			custom,
			indexed_custom,
		};

	private:
		const lexer_action_type _type;

	protected:
		lexer_action(lexer_action_type type)
			: _type(type)
		{
		}

	public:
		lexer_action_type action_type() const
		{
			return _type;
		}

		bool position_dependent() const
		{
			return action_type() >= lexer_action_type::custom;
		}

	public:
		class channel_action;
		class mode_action;
		class more_action;
		class pop_mode_action;
		class push_mode_action;
		class skip_action;
		class type_action;
		class custom_action;
		class indexed_custom_action;
	};

	class lexer_action::channel_action : public lexer_action
	{
	private:
		channel_action(channel_action const&) = delete;
		channel_action& operator= (channel_action const&) = delete;

	private:
		const int32_t _channel;

	public:
		explicit channel_action(int32_t channel)
			: lexer_action(lexer_action_type::channel)
			, _channel(channel)
		{
		}

		int32_t channel() const
		{
			return _channel;
		}
	};

	class lexer_action::mode_action : public lexer_action
	{
	private:
		mode_action(mode_action const&) = delete;
		mode_action& operator= (mode_action const&) = delete;

	private:
		const int32_t _mode;

	public:
		explicit mode_action(int32_t mode)
			: lexer_action(lexer_action_type::mode)
			, _mode(mode)
		{
		}

		int32_t mode() const
		{
			return _mode;
		}
	};

	class lexer_action::more_action : public lexer_action
	{
	private:
		more_action(more_action const&) = delete;
		more_action& operator= (more_action const&) = delete;

	protected:
		explicit more_action();

	public:
		static const std::shared_ptr<more_action> instance;
	};

	class lexer_action::pop_mode_action : public lexer_action
	{
	private:
		pop_mode_action(pop_mode_action const&) = delete;
		pop_mode_action& operator= (pop_mode_action const&) = delete;

	protected:
		explicit pop_mode_action();

	public:
		static const std::shared_ptr<pop_mode_action> instance;
	};

	class lexer_action::push_mode_action : public lexer_action
	{
	private:
		push_mode_action(push_mode_action const&) = delete;
		push_mode_action& operator= (push_mode_action const&) = delete;

	private:
		const int32_t _mode;

	public:
		explicit push_mode_action(int32_t mode)
			: lexer_action(lexer_action_type::push_mode)
			, _mode(mode)
		{
		}

		int32_t mode() const
		{
			return _mode;
		}
	};

	class lexer_action::skip_action : public lexer_action
	{
	private:
		skip_action(skip_action const&) = delete;
		skip_action& operator= (skip_action const&) = delete;

	protected:
		explicit skip_action();

	public:
		static const std::shared_ptr<skip_action> instance;
	};

	class lexer_action::type_action : public lexer_action
	{
	private:
		type_action(type_action const&) = delete;
		type_action& operator= (type_action const&) = delete;

	private:
		const int32_t _type;

	public:
		explicit type_action(int32_t type)
			: lexer_action(lexer_action_type::type)
			, _type(type)
		{
		}

		int32_t type() const
		{
			return _type;
		}
	};

	class lexer_action::custom_action : public lexer_action
	{
	private:
		custom_action(custom_action const&) = delete;
		custom_action& operator= (custom_action const&) = delete;

	private:
		const size_t _rule_index;
		const size_t _action_index;

	public:
		explicit custom_action(size_t rule_index, size_t action_index)
			: lexer_action(lexer_action_type::custom)
			, _rule_index(rule_index)
			, _action_index(action_index)
		{
		}

		size_t rule_index() const
		{
			return _rule_index;
		}

		size_t action_index() const
		{
			return _action_index;
		}
	};

	class lexer_action::indexed_custom_action : public lexer_action
	{
	private:
		indexed_custom_action(indexed_custom_action const&) = delete;
		indexed_custom_action& operator= (indexed_custom_action const&) = delete;

	private:
		const size_t _offset;
		const std::shared_ptr<lexer_action> _action;

	public:
		indexed_custom_action(size_t offset, std::shared_ptr<lexer_action> const& action)
			: lexer_action(lexer_action_type::indexed_custom)
			, _offset(offset)
			, _action(action)
		{
		}

	public:
		size_t offset() const
		{
			return _offset;
		}

		std::shared_ptr<lexer_action> const& action() const
		{
			return _action;
		}
	};

	bool operator== (lexer_action const& x, lexer_action const& y);

}
}

namespace std {

	template<>
	struct hash<antlr4::atn::lexer_action>
	{
		size_t operator() (antlr4::atn::lexer_action const& action) const;
	};

}
