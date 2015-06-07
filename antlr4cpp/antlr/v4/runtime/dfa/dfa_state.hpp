// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <memory>
#include <vector>

#include "accept_state_information.hpp"

namespace antlr4 {

	namespace atn {
		class atn_configuration_set;
		class grammar_atn;
		class semantic_context;
	}

namespace dfa {

	class dynamic_dfa;

	class dfa_state
	{
	private:
		class predicate_prediction
		{
		private:
			const std::shared_ptr<atn::semantic_context> _predicate;
			const size_t _alternative;

		public:
			predicate_prediction(std::shared_ptr<atn::semantic_context> predicate, size_t alternative)
				: _predicate(predicate)
				, _alternative(alternative)
			{
			}

		public:
			std::shared_ptr<atn::semantic_context> const& predicate() const
			{
				return _predicate;
			}

			size_t alternative() const
			{
				return _alternative;
			}
		};

	private:
		size_t _state_number;

		std::shared_ptr<atn::atn_configuration_set> _configurations;

		accept_state_information _accept_state_information;

		const int32_t _min_dfa_edge;
		std::vector<std::shared_ptr<dfa_state>> _edges;

		std::vector<std::shared_ptr<dfa_state>> _context_edges;

		std::vector<bool> _context_symbols;

		std::vector<predicate_prediction> _predicates;

	public:
		dfa_state(std::shared_ptr<dynamic_dfa> const& dfa, std::shared_ptr<atn::atn_configuration_set> const& configurations);

	public:
		bool context_sensitive() const
		{
			return !_context_symbols.empty();
		}

		bool is_context_symbol(int32_t symbol) const;
		void set_context_symbol(int32_t symbol);
		void set_context_sensitive(std::shared_ptr<atn::grammar_atn> const& atn);

		dfa::accept_state_information const& accept_state_information() const
		{
			return _accept_state_information;
		}

		void accept_state_information(dfa::accept_state_information const& value)
		{
			_accept_state_information = value;
		}

		bool accept_state() const;
		size_t prediction() const;

		std::shared_ptr<dfa_state> const& target(int32_t symbol) const;

		void set_target(int32_t symbol, std::shared_ptr<dfa_state> const& value);

		std::shared_ptr<dfa_state> const& context_target(size_t invoking_state) const;

		void set_context_target(size_t invoking_state, std::shared_ptr<dfa_state> const& value);
	};

	bool operator== (dfa_state const& x, dfa_state const& y);

}
}

namespace std {

	template<>
	struct hash<antlr4::dfa::dfa_state>
	{
		size_t operator() (antlr4::dfa::dfa_state const& value) const;
	};

}
