#pragma once

#include <vector>

namespace antlr4 {
namespace runtime {
namespace atn {

	class conflict_information
	{
	private:
		conflict_information() = delete;
		conflict_information(conflict_information const&) = delete;
		conflict_information& operator= (conflict_information const&) = delete;

	private:
		const std::vector<bool> _conflicted_alternatives;
		const bool _exact;

	public:
		conflict_information(std::vector<bool>&& conflicted_alternatives, bool exact)
			: _conflicted_alternatives(conflicted_alternatives)
			, _exact(exact)
		{
		}

	public:
		std::vector<bool> const& conflicted_alternatives() const
		{
			return _conflicted_alternatives;
		}

		bool exact() const
		{
			return _exact;
		}
	};

	bool operator== (conflict_information const& x, conflict_information const& y);

}
}
}

namespace std {

	template<>
	struct hash<antlr4::runtime::atn::conflict_information>
	{
		bool operator() (antlr4::runtime::atn::conflict_information const& x) const;
	};

}
