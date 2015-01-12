#pragma once

#include <memory>
#include <vector>

namespace antlr4 {
namespace runtime {
namespace atn {

	class lexer_action;

	class lexer_action_executor
	{
	private:
		std::vector<std::shared_ptr<lexer_action>> _actions;
		const size_t _hash_code;

		friend bool operator== (lexer_action_executor const&, lexer_action_executor const&);
		friend std::hash<lexer_action_executor>;

	public:
		lexer_action_executor(std::vector<std::shared_ptr<lexer_action>>&& actions);

	public:
		std::vector<std::shared_ptr<lexer_action>> const& actions() const
		{
			return _actions;
		}

	public:
		static std::shared_ptr<lexer_action_executor> append(std::shared_ptr<lexer_action_executor> const& executor, std::shared_ptr<lexer_action> const& action);
		static std::shared_ptr<lexer_action_executor> fix_offset_before_match(std::shared_ptr<lexer_action_executor> const& executor, size_t offset);
	};

	bool operator== (lexer_action_executor const& x, lexer_action_executor const& y);

}
}
}

namespace std {

	template<>
	struct hash<antlr4::runtime::atn::lexer_action_executor>
	{
		size_t operator() (antlr4::runtime::atn::lexer_action_executor const& executor) const
		{
			return executor._hash_code;
		}
	};

}

