// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <iterator>

#include "atn_deserializer.hpp"

#include "atn_state.hpp"
#include "atn_type.hpp"
#include "lexer_action.hpp"
#include "grammar_atn.hpp"
#include "transition.hpp"

#include "../token.hpp"
#include "../dfa/dynamic_dfa.hpp"
#include "../misc/interval_set.hpp"
#include "../misc/uuid.hpp"

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) )
#endif

namespace antlr4 {
namespace atn {

	using namespace dfa;
	using namespace misc;

	namespace {

		const size_t invalid_rule_index = ~static_cast<size_t>(0);
		const size_t invalid_decision_index = ~static_cast<size_t>(0);

		int32_t to_int(uint16_t value)
		{
			return value;
		}

		int32_t to_int32(std::vector<uint16_t> const& data, size_t offset)
		{
			return static_cast<int32_t>(data[offset]) | (static_cast<int32_t>(data[offset + 1]) << 16);
		}

		int64_t to_int64(std::vector<uint16_t> const& data, size_t offset)
		{
			uint32_t low_bytes = static_cast<uint32_t>(to_int32(data, offset));
			uint32_t high_bytes = static_cast<uint32_t>(to_int32(data, offset + 2));
			uint64_t result = static_cast<uint64_t>(low_bytes) | (static_cast<uint64_t>(high_bytes) << 32);
			return static_cast<int64_t>(result);
		}

		uuid to_uuid(std::vector<uint16_t> const& data, size_t offset)
		{
			int32_t a = to_int32(data, offset + 4 + 2);
			int16_t b = static_cast<int16_t>(data[offset + 4 + 1]);
			int16_t c = static_cast<int16_t>(data[offset + 4]);

			int64_t rest = to_int64(data, offset);
			uint8_t const* d = reinterpret_cast<uint8_t const*>(&rest);

			return uuid(a, b, c, d[7], d[6], d[5], d[4], d[3], d[2], d[1], d[0]);
		}

		const int32_t serialized_version = 3;

		// E4178468-DF95-44D0-AD87-F22A5D5FB6D3
		uuid base_serialized_uuid(0xE4178468, 0xDF95, 0x44D0, 0xAD, 0x87, 0xF2, 0x2A, 0x5D, 0x5F, 0xB6, 0xD3);

		// AB35191A-1603-487E-B75A-479B831EAF6D
		uuid added_lexer_actions(0xAB35191A, 0x1603, 0x487E, 0xB7, 0x5A, 0x47, 0x9B, 0x83, 0x1E, 0xAF, 0x6D);

		std::vector<uuid> supported_uuids{ base_serialized_uuid, added_lexer_actions };

		uuid serialized_uuid = added_lexer_actions;

		bool is_feature_supported(uuid feature_uuid, uuid atn_uuid)
		{
			auto feature_index = std::find(supported_uuids.begin(), supported_uuids.end(), feature_uuid);
			if (feature_index == supported_uuids.end())
			{
				return false;
			}

			auto atn_index = std::find(supported_uuids.begin(), supported_uuids.end(), atn_uuid);
			if (atn_index == supported_uuids.end())
			{
				return false;
			}

			return atn_index >= feature_index;
		}

		void mark_precedence_decisions(std::shared_ptr<grammar_atn> const& atn)
		{
			for each (std::shared_ptr<atn_state> const& state in atn->states())
			{
				if (state->state_type() != atn_state::atn_state_type::star_loop_entry)
				{
					continue;
				}

				/* We analyze the ATN to determine if this ATN decision state is the
				 * decision for the closure block that determines whether a
				 * precedence rule should continue or complete.
				 */
				if (atn->rule_start_states()[state->rule_index()]->precedence_rule())
				{
					std::shared_ptr<atn_state> const& maybe_loop_end_state = state->transition(state->transitions().size() - 1)->target();
					if (maybe_loop_end_state->state_type() == atn_state::atn_state_type::loop_end)
					{
						if (maybe_loop_end_state->only_has_epsilon_transitions() && maybe_loop_end_state->transition(0)->target()->state_type() == atn_state::atn_state_type::rule_stop)
						{
							std::static_pointer_cast<star_loop_entry_state>(state)->precedence_rule_decision(true);
						}
					}
				}
			}
		}

		bool is_block_start_state(std::shared_ptr<atn_state> const& state)
		{
			atn_state::atn_state_type state_type = state->state_type();
			return state_type == atn_state::atn_state_type::block_start
				|| state_type == atn_state::atn_state_type::plus_block_start
				|| state_type == atn_state::atn_state_type::star_block_start;
		}

		bool is_decision_state(std::shared_ptr<atn_state> const& state)
		{
			atn_state::atn_state_type state_type = state->state_type();
			return state_type == atn_state::atn_state_type::block_start
				|| state_type == atn_state::atn_state_type::plus_block_start
				|| state_type == atn_state::atn_state_type::plus_loop_back
				|| state_type == atn_state::atn_state_type::star_block_start
				|| state_type == atn_state::atn_state_type::star_loop_entry
				|| state_type == atn_state::atn_state_type::token_start;
		}

		void check_condition(bool condition, char const* message)
		{
			if (!condition)
			{
				throw std::runtime_error(message);
			}
		}

		void check_condition(bool condition)
		{
			check_condition(condition, "ATN verification failed");
		}

		void verify_atn(std::shared_ptr<grammar_atn> const& atn)
		{
			// verify assumptions
			for each (std::shared_ptr<atn_state> const& state in atn->states())
			{
				if (!state)
				{
					continue;
				}

				check_condition(state->only_has_epsilon_transitions() || state->transitions().size() <= 1);

				if (state->state_type() == atn_state::atn_state_type::plus_block_start)
				{
					check_condition(!!std::static_pointer_cast<plus_block_start_state>(state)->loopback_state());
				}

				if (state->state_type() == atn_state::atn_state_type::star_loop_entry)
				{
					std::shared_ptr<star_loop_entry_state> star_loop_entry_state = std::static_pointer_cast<atn::star_loop_entry_state>(state);
					check_condition(!!star_loop_entry_state->loopback_state());
					check_condition(star_loop_entry_state->transitions().size() == 2);

					if (star_loop_entry_state->transition(0)->target()->state_type() == atn_state::atn_state_type::star_block_start)
					{
						check_condition(star_loop_entry_state->transition(1)->target()->state_type() == atn_state::atn_state_type::loop_end);
						check_condition(star_loop_entry_state->greedy());
					}
					else if (star_loop_entry_state->transition(0)->target()->state_type() == atn_state::atn_state_type::loop_end)
					{
						check_condition(star_loop_entry_state->transition(1)->target()->state_type() == atn_state::atn_state_type::star_block_start);
						check_condition(!star_loop_entry_state->greedy());
					}
					else
					{
						check_condition(false);
					}
				}

				if (state->state_type() == atn_state::atn_state_type::star_loop_back)
				{
					check_condition(state->transitions().size() == 1);
					check_condition(state->transition(0)->target()->state_type() == atn_state::atn_state_type::star_loop_entry);
				}

				if (state->state_type() == atn_state::atn_state_type::loop_end)
				{
					check_condition(!!std::static_pointer_cast<loop_end_state>(state)->loopback_state());
				}

				if (state->state_type() == atn_state::atn_state_type::rule_start)
				{
					check_condition(!!std::static_pointer_cast<rule_start_state>(state)->stop_state());
				}

				if (is_block_start_state(state))
				{
					check_condition(!!std::static_pointer_cast<block_start_state>(state)->end_state());
				}

				if (state->state_type() == atn_state::atn_state_type::block_end)
				{
					check_condition(!!std::static_pointer_cast<block_end_state>(state)->start_state());
				}

				if (is_decision_state(state))
				{
					std::shared_ptr<decision_state> decision_state(std::static_pointer_cast<decision_state>(state));
					check_condition(decision_state->transitions().size() <= 1 || decision_state->decision() != invalid_decision_index);
				}
				else
				{
					check_condition(state->transitions().size() <= 1 || state->state_type() == atn_state::atn_state_type::rule_stop);
				}
			}
		}

		size_t inline_set_rules(std::shared_ptr<grammar_atn> const& atn)
		{
			size_t inlined_calls = 0;

			std::vector<std::shared_ptr<transition>> rule_to_inline_transition(atn->rule_start_states().size());
			for (size_t i = 0; i < atn->rule_start_states().size(); i++)
			{
				std::shared_ptr<rule_start_state> const& start_state = atn->rule_start_states()[i];
				std::shared_ptr<atn_state> middle_state(start_state);
				while (middle_state->only_has_epsilon_transitions()
					&& middle_state->optimized_transitions().size() == 1
					&& middle_state->optimized_transition(0)->type() == transition::transition_type::epsilon)
				{
					middle_state = middle_state->optimized_transition(0)->target();
				}

				if (middle_state->optimized_transitions().size() != 1)
				{
					continue;
				}

				std::shared_ptr<transition> const& match_transition(middle_state->optimized_transition(0));
				std::shared_ptr<atn_state> const& match_target(match_transition->target());
				if (match_transition->epsilon()
					|| !match_target->only_has_epsilon_transitions()
					|| match_target->optimized_transitions().size() != 1
					|| match_target->optimized_transition(0)->target()->state_type() != atn_state::atn_state_type::rule_stop)
				{
					continue;
				}

				switch (match_transition->type())
				{
				case transition::transition_type::atom:
				case transition::transition_type::range:
				case transition::transition_type::set:
					rule_to_inline_transition[i] = match_transition;
					break;

				case transition::transition_type::not_set:
				case transition::transition_type::wildcard:
					// not implemented yet
					continue;

				case transition::transition_type::epsilon:
				case transition::transition_type::precedence:
				case transition::transition_type::predicate:
				case transition::transition_type::action:
				case transition::transition_type::rule:
				default:
					continue;
				}
			}

			for (size_t state_number = 0; state_number < atn->states().size(); state_number++)
			{
				std::shared_ptr<atn_state> const& state = atn->states()[state_number];
				if (state->rule_index() == invalid_rule_index)
				{
					continue;
				}

				std::vector<std::shared_ptr<transition>> optimized_transitions;
				for (size_t i = 0; i < state->optimized_transitions().size(); i++) {
					std::shared_ptr<transition> const& transition = state->optimized_transition(i);
					if (transition->type() != transition::transition_type::rule) {
						if (!optimized_transitions.empty())
						{
							optimized_transitions.push_back(transition);
						}

						continue;
					}

					std::shared_ptr<rule_transition> rule_transition(std::static_pointer_cast<rule_transition>(transition));
					std::shared_ptr<atn::transition> const& effective = rule_to_inline_transition[rule_transition->target()->rule_index()];
					if (!effective)
					{
						if (!optimized_transitions.empty())
						{
							optimized_transitions.push_back(transition);
						}

						continue;
					}

					if (optimized_transitions.empty() && !!i)
					{
						std::copy(state->optimized_transitions().begin(), state->optimized_transitions().end(), std::back_inserter(optimized_transitions));
					}

					inlined_calls++;
					std::shared_ptr<atn_state> const& target = rule_transition->follow_state();
					std::shared_ptr<atn_state> intermediate_state(std::make_shared<basic_state>(atn, atn->states().size(), target->rule_index()));
					atn->add_state(intermediate_state);
					optimized_transitions.push_back(std::make_shared<epsilon_transition>(intermediate_state));

					switch (effective->type())
					{
					case transition::transition_type::atom:
						intermediate_state->add_transition(std::make_shared<atom_transition>(target, std::static_pointer_cast<atom_transition>(effective)->label()));
						break;

					case transition::transition_type::range:
						intermediate_state->add_transition(std::make_shared<range_transition>(target, std::static_pointer_cast<range_transition>(effective)->label()));
						break;

					case transition::transition_type::set:
						intermediate_state->add_transition(std::make_shared<set_transition>(target, std::static_pointer_cast<set_transition>(effective)->label()));
						break;

					case transition::transition_type::not_set:
					case transition::transition_type::wildcard:
					case transition::transition_type::epsilon:
					case transition::transition_type::precedence:
					case transition::transition_type::predicate:
					case transition::transition_type::action:
					case transition::transition_type::rule:
					default:
						throw std::runtime_error("unsupported operation");
					}
				}

				if (!optimized_transitions.empty())
				{
					if (state->optimized())
					{
						while (!state->optimized_transitions().empty())
						{
							state->remove_optimized_transition(state->optimized_transitions().size() - 1);
						}
					}

					for each (std::shared_ptr<transition> const& transition in optimized_transitions)
					{
						state->add_optimized_transition(state->optimized_transitions().size(), transition);
					}
				}
			}

			//if (ParserATNSimulator.debug) {
			//	System.out.println("ATN runtime optimizer removed " + inlinedCalls + " rule invocations by inlining sets.");
			//}

			return inlined_calls;
		}

		size_t combine_chained_epsilons(std::shared_ptr<grammar_atn> const& atn)
		{
			size_t removed_edges = 0;

			//nextState:
			for each (std::shared_ptr<atn_state> const& state in atn->states())
			{
				if (!state->only_has_epsilon_transitions() || state->state_type() == atn_state::atn_state_type::rule_stop)
				{
					continue;
				}

				std::vector<std::shared_ptr<transition>> optimized_transitions;
				//nextTransition:
				for (size_t i = 0; i < state->optimized_transitions().size(); i++)
				{
					std::shared_ptr<transition> const& transition = state->optimized_transition(i);
					std::shared_ptr<atn_state> const& intermediate = transition->target();
					if (transition->type() != transition::transition_type::epsilon
						|| std::static_pointer_cast<epsilon_transition>(transition)->outermost_precedence_return() != -1
						|| intermediate->state_type() != atn_state::atn_state_type::basic
						|| !intermediate->only_has_epsilon_transitions())
					{
						if (!optimized_transitions.empty())
						{
							optimized_transitions.push_back(transition);
						}

						continue;
					}

					bool continue_next_transition = false;
					for (size_t j = 0; j < intermediate->optimized_transitions().size(); j++)
					{
						if (intermediate->optimized_transition(j)->type() != transition::transition_type::epsilon
							|| std::static_pointer_cast<epsilon_transition>(intermediate->optimized_transition(j))->outermost_precedence_return() != -1)
						{
							if (!optimized_transitions.empty())
							{
								optimized_transitions.push_back(transition);
							}

							continue_next_transition = true;
							break;
						}
					}

					if (continue_next_transition)
					{
						continue;
					}

					removed_edges++;
					if (optimized_transitions.empty() && !!i)
					{
						std::copy(state->optimized_transitions().begin(), state->optimized_transitions().end(), std::back_inserter(optimized_transitions));
					}

					for (size_t j = 0; j < intermediate->optimized_transitions().size(); j++)
					{
						std::shared_ptr<atn_state> const& target = intermediate->optimized_transition(j)->target();
						optimized_transitions.push_back(std::make_shared<epsilon_transition>(target));
					}
				}

				if (!optimized_transitions.empty())
				{
					if (state->optimized())
					{
						while (!state->optimized_transitions().empty())
						{
							state->remove_optimized_transition(state->optimized_transitions().size() - 1);
						}
					}

					for each (std::shared_ptr<transition> const& transition in optimized_transitions)
					{
						state->add_optimized_transition(state->optimized_transitions().size(), transition);
					}
				}
			}

			//if (ParserATNSimulator.debug) {
			//	System.out.println("ATN runtime optimizer removed " + removedEdges + " transitions by combining chained epsilon transitions.");
			//}

			return removed_edges;
		}

		size_t optimize_sets(std::shared_ptr<grammar_atn> const& atn, bool preserve_order)
		{
			if (preserve_order)
			{
				// this optimization currently doesn't preserve edge order.
				return 0;
			}

			size_t removed_paths = 0;
			std::vector<std::shared_ptr<decision_state>> const& decisions = atn->decisions();
			for each (std::shared_ptr<decision_state> const& decision in decisions)
			{
				interval_set<size_t> set_transitions;
				for (size_t i = 0; i < decision->optimized_transitions().size(); i++)
				{
					std::shared_ptr<transition> const& epsilon_transition = decision->optimized_transition(i);
					if (epsilon_transition->type() != transition::transition_type::epsilon)
					{
						continue;
					}

					if (epsilon_transition->target()->optimized_transitions().size() != 1)
					{
						continue;
					}

					std::shared_ptr<transition> const& transition = epsilon_transition->target()->optimized_transition(0);
					if (transition->target()->state_type() != atn_state::atn_state_type::block_end)
					{
						continue;
					}

					if (transition->type() == transition::transition_type::not_set)
					{
						// TODO: not yet implemented
						continue;
					}

					if (transition->type() == transition::transition_type::atom
						|| transition->type() == transition::transition_type::range
						|| transition->type() == transition::transition_type::set)
					{
						set_transitions.insert(i);
					}
				}

				if (set_transitions.size() <= 1)
				{
					continue;
				}

				std::vector<std::shared_ptr<transition>> optimized_transitions;
				for (size_t i = 0; i < decision->optimized_transitions().size(); i++)
				{
					if (!set_transitions.contains(i))
					{
						optimized_transitions.push_back(decision->optimized_transition(i));
					}
				}

				std::shared_ptr<atn_state> const& block_end_state(decision->optimized_transition(set_transitions.min())->target()->optimized_transition(0)->target());
				interval_set<int32_t> match_set;
				for (size_t i = 0; i < set_transitions.pairs().size(); i++)
				{
					std::pair<size_t, size_t> interval = set_transitions.pairs()[i];
					for (size_t j = interval.first; j <= interval.second; j++)
					{
						std::shared_ptr<transition> const& match_transition = decision->optimized_transition(j)->target()->optimized_transition(0);
						switch (match_transition->type())
						{
						case transition::transition_type::not_set:
							throw std::runtime_error("Not yet implemented (by reference version).");

						case transition::transition_type::atom:
							match_set.insert(std::static_pointer_cast<atom_transition>(match_transition)->label());
							break;

						case transition::transition_type::range:
							match_set.insert(std::static_pointer_cast<range_transition>(match_transition)->label());
							break;

						case transition::transition_type::set:
							match_set.insert(std::static_pointer_cast<set_transition>(match_transition)->label());
							break;

						case transition::transition_type::wildcard:
						case transition::transition_type::epsilon:
						case transition::transition_type::precedence:
						case transition::transition_type::predicate:
						case transition::transition_type::action:
						case transition::transition_type::rule:
						default:
							break;
						}
					}
				}

				std::shared_ptr<transition> new_transition;
				if (match_set.pairs().size() == 1)
				{
					if (match_set.size() == 1)
					{
						new_transition = std::make_shared<atom_transition>(block_end_state, match_set.min());
					}
					else
					{
						new_transition = std::make_shared<range_transition>(block_end_state, match_set.pairs()[0]);
					}
				}
				else
				{
					new_transition = std::make_shared<set_transition>(block_end_state, match_set);
				}

				std::shared_ptr<atn_state> set_optimized_state(std::make_shared<basic_state>(atn, atn->states().size(), decision->rule_index()));
				atn->add_state(set_optimized_state);

				set_optimized_state->add_transition(new_transition);
				optimized_transitions.push_back(std::make_shared<epsilon_transition>(set_optimized_state));

				removed_paths += decision->optimized_transitions().size() - optimized_transitions.size();

				if (decision->optimized())
				{
					while (!decision->optimized_transitions().empty())
					{
						decision->remove_optimized_transition(decision->optimized_transitions().size() - 1);
					}
				}

				for each (std::shared_ptr<transition> const& transition in optimized_transitions)
				{
					decision->add_optimized_transition(decision->optimized_transitions().size(), transition);
				}
			}

			//if (ParserATNSimulator.debug) {
			//	System.out.println("ATN runtime optimizer removed " + removedPaths + " paths by collapsing sets.");
			//}

			return removed_paths;
		}

		bool test_tail_call(std::shared_ptr<grammar_atn> const& atn, std::shared_ptr<rule_transition> const& transition, bool optimized_path)
		{
			if (!optimized_path && transition->tail_call())
			{
				return true;
			}

			if (optimized_path && transition->optimized_tail_call())
			{
				return true;
			}

			std::vector<bool> reachable(atn->states().size());
			std::deque<std::shared_ptr<atn_state>> worklist{ transition->follow_state() };
			while (!worklist.empty())
			{
				std::shared_ptr<atn_state> state(std::move(worklist.back()));
				worklist.pop_back();
				if (reachable[state->state_number()])
				{
					continue;
				}

				if (state->state_type() == atn_state::atn_state_type::rule_stop)
				{
					continue;
				}

				if (!state->only_has_epsilon_transitions())
				{
					return false;
				}

				std::vector<std::shared_ptr<atn::transition>> const& transitions = optimized_path ? state->optimized_transitions() : state->transitions();
				for each (std::shared_ptr<atn::transition> const& t in transitions)
				{
					if (t->type() != transition::transition_type::epsilon)
					{
						return false;
					}

					worklist.push_back(t->target());
				}
			}

			return true;
		}

		void identify_tail_calls(std::shared_ptr<grammar_atn> const& atn)
		{
			for each (std::shared_ptr<atn_state> const& state in atn->states())
			{
				for each (std::shared_ptr<transition> const& transition in state->transitions())
				{
					if (transition->type() != transition::transition_type::rule)
					{
						continue;
					}

					std::shared_ptr<rule_transition> rule_transition(std::static_pointer_cast<rule_transition>(transition));
					rule_transition->tail_call(test_tail_call(atn, rule_transition, false));
					rule_transition->optimized_tail_call(test_tail_call(atn, rule_transition, true));
				}

				if (!state->optimized())
				{
					continue;
				}

				for each (std::shared_ptr<transition> const& transition in state->optimized_transitions())
				{
					if (transition->type() != transition::transition_type::rule)
					{
						continue;
					}

					std::shared_ptr<rule_transition> rule_transition(std::static_pointer_cast<rule_transition>(transition));
					rule_transition->tail_call(test_tail_call(atn, rule_transition, false));
					rule_transition->optimized_tail_call(test_tail_call(atn, rule_transition, true));
				}
			}
		}

		std::shared_ptr<atn_state> state_factory(std::shared_ptr<atn::grammar_atn> const& grammar_atn, atn_state::atn_state_type type, size_t state_number, size_t rule_index)
		{
			switch (type)
			{
			case atn_state::atn_state_type::invalid:
				return nullptr;

			case atn_state::atn_state_type::basic:
				return std::make_shared<basic_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::rule_start:
				return std::make_shared<rule_start_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::block_start:
				return std::make_shared<basic_block_start_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::plus_block_start:
				return std::make_shared<plus_block_start_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::star_block_start:
				return std::make_shared<star_block_start_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::token_start:
				return std::make_shared<tokens_start_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::rule_stop:
				return std::make_shared<rule_stop_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::block_end:
				return std::make_shared<block_end_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::star_loop_back:
				return std::make_shared<star_loopback_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::star_loop_entry:
				return std::make_shared<star_loop_entry_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::plus_loop_back:
				return std::make_shared<plus_loopback_state>(grammar_atn, state_number, rule_index);

			case atn_state::atn_state_type::loop_end:
				return std::make_shared<loop_end_state>(grammar_atn, state_number, rule_index);

			default:
				assert(!"Invalid state type.");
				return nullptr;
			}
		}

		std::shared_ptr<transition> edge_factory(std::shared_ptr<grammar_atn> const& atn, transition::transition_type transition_type, size_t /*source_state_number*/, size_t target_state_number, int32_t arg1, int32_t arg2, int32_t arg3, std::vector<interval_set<int32_t>> const& sets)
		{
			std::shared_ptr<atn_state> const& target = atn->states()[target_state_number];
			switch (transition_type)
			{
			case transition::transition_type::epsilon:
				return std::make_shared<epsilon_transition>(target);

			case transition::transition_type::range:
				// the serialized ATN uses inclusive edges, but range_transition uses an exclusive max edge
				return std::make_shared<range_transition>(target, std::make_pair(arg3 ? token::eof : arg1, arg2 + 1));

			case transition::transition_type::rule:
				return std::make_shared<rule_transition>(std::static_pointer_cast<rule_start_state>(atn->states()[static_cast<size_t>(arg1)]), arg2, arg3, target);

			case transition::transition_type::predicate:
				return std::make_shared<predicate_transition>(target, arg1, arg2, !!arg3);

			case transition::transition_type::precedence:
				return std::make_shared<precedence_transition>(target, arg1);

			case transition::transition_type::atom:
				return std::make_shared<atom_transition>(target, arg3 ? token::eof : arg1);

			case transition::transition_type::action:
				return std::make_shared<action_transition>(target, arg1, arg2, !!arg3);

			case transition::transition_type::set:
				return std::make_shared<set_transition>(target, sets[static_cast<size_t>(arg1)]);

			case transition::transition_type::not_set:
				return std::make_shared<not_set_transition>(target, sets[static_cast<size_t>(arg1)]);

			case transition::transition_type::wildcard:
				return std::make_shared<wildcard_transition>(target);

			default:
				assert(!"Invalid transition type.");
				return nullptr;
			}
		}

		std::shared_ptr<lexer_action> lexer_action_factory(lexer_action::lexer_action_type type, int32_t data1, int32_t data2)
		{
			switch (type)
			{
			case lexer_action::lexer_action_type::channel:
				return std::make_shared<lexer_action::channel_action>(data1);

			case lexer_action::lexer_action_type::custom:
				return std::make_shared<lexer_action::custom_action>(data1, data2);

			case lexer_action::lexer_action_type::mode:
				return std::make_shared<lexer_action::mode_action>(data1);

			case lexer_action::lexer_action_type::more:
				return lexer_action::more_action::instance;

			case lexer_action::lexer_action_type::pop_mode:
				return lexer_action::pop_mode_action::instance;

			case lexer_action::lexer_action_type::push_mode:
				return std::make_shared<lexer_action::push_mode_action>(data1);

			case lexer_action::lexer_action_type::skip:
				return lexer_action::skip_action::instance;

			case lexer_action::lexer_action_type::type:
				return std::make_shared<lexer_action::type_action>(data1);

			case lexer_action::lexer_action_type::indexed_custom:
			default:
				assert(!"Invalid action type.");
				return nullptr;
			}
		}

	}

	std::shared_ptr<grammar_atn> atn_deserializer::deserialize(std::vector<uint16_t> const& raw_data) const
	{
		std::vector<uint16_t> data(raw_data);

		// don't adjust the first value since that's the version number
		for (size_t i = 1; i < data.size(); i++)
		{
			data[i] = static_cast<uint16_t>(data[i] - 2);
		}

		size_t p = 0;
		int32_t version = to_int(data[p++]);
		if (version != serialized_version)
		{
			//String reason = String.format(Locale.getDefault(), "Could not deserialize ATN with version %d (expected %d).", version, SERIALIZED_VERSION);
			throw std::runtime_error("error message not implemented");
		}

		uuid guid = to_uuid(data, p);
		p += 8;
		if (std::find(supported_uuids.begin(), supported_uuids.end(), guid) == supported_uuids.end())
		{
			//String reason = String.format(Locale.getDefault(), "Could not deserialize ATN with UUID %s (expected %s or a legacy UUID).", uuid, SERIALIZED_UUID);
			throw std::runtime_error("error message not implemented");
		}

		bool supports_lexer_actions = is_feature_supported(added_lexer_actions, guid);

		atn_type grammar_type = static_cast<atn_type>(to_int(data[p++]));
		int32_t max_token_type = to_int(data[p++]);
		std::shared_ptr<grammar_atn> atn(std::make_shared<grammar_atn>(grammar_type, max_token_type));

		//
		// STATES
		//
		std::vector<std::pair<std::shared_ptr<loop_end_state>, size_t>> loop_back_state_numbers;
		std::vector<std::pair<std::shared_ptr<block_start_state>, size_t>> end_state_numbers;
		size_t nstates = static_cast<size_t>(to_int(data[p++]));
		for (size_t i = 0; i < nstates; i++)
		{
			atn_state::atn_state_type state_type = static_cast<atn_state::atn_state_type>(to_int(data[p++]));
			// ignore bad type of states
			if (state_type == atn_state::atn_state_type::invalid)
			{
				atn->add_state(nullptr);
				continue;
			}

			size_t rule_index = static_cast<size_t>(to_int(data[p++]));
			if (rule_index == std::numeric_limits<uint16_t>::max())
			{
				rule_index = ~static_cast<size_t>(0);
			}

			std::shared_ptr<atn_state> state(state_factory(atn, state_type, atn->states().size(), rule_index));
			if (state_type == atn_state::atn_state_type::loop_end)
			{
				// special case
				size_t loop_back_state_number = static_cast<size_t>(to_int(data[p++]));
				loop_back_state_numbers.push_back(std::make_pair(std::static_pointer_cast<loop_end_state>(state), loop_back_state_number));
			}
			else if (state->state_type() == atn_state::atn_state_type::block_start)
			{
				size_t end_state_number = static_cast<size_t>(to_int(data[p++]));
				end_state_numbers.push_back(std::make_pair(std::static_pointer_cast<block_start_state>(state), end_state_number));
			}

			atn->add_state(state);
		}

		// delay the assignment of loop back and end states until we know all the state instances have been initialized
		for each (auto pair in loop_back_state_numbers)
		{
			pair.first->loopback_state(atn->states()[pair.second]);
		}

		for each (auto pair in end_state_numbers)
		{
			pair.first->end_state(std::static_pointer_cast<block_end_state>(atn->states()[pair.second]));
		}

		size_t num_non_greedy_states = static_cast<size_t>(to_int(data[p++]));
		for (size_t i = 0; i < num_non_greedy_states; i++)
		{
			size_t state_number = static_cast<size_t>(to_int(data[p++]));
			std::static_pointer_cast<decision_state>(atn->states()[state_number])->greedy(false);
		}

		size_t num_sll_decisions = static_cast<size_t>(to_int(data[p++]));
		for (size_t i = 0; i < num_sll_decisions; i++)
		{
			size_t state_number = static_cast<size_t>(to_int(data[p++]));
			std::static_pointer_cast<decision_state>(atn->states()[state_number])->sll(true);
		}

		size_t num_precedence_states = static_cast<size_t>(to_int(data[p++]));
		for (size_t i = 0; i < num_precedence_states; i++)
		{
			size_t state_number = static_cast<size_t>(to_int(data[p++]));
			std::static_pointer_cast<rule_start_state>(atn->states()[state_number])->precedence_rule(true);
		}

		//
		// RULES
		//
		size_t nrules = static_cast<size_t>(to_int(data[p++]));
		if (atn->grammar_type() == atn_type::lexer)
		{
			atn->_rule_to_token_type.resize(nrules);
		}

		atn->_rule_start_states.resize(nrules);
		for (size_t i = 0; i < nrules; i++)
		{
			size_t state_number = static_cast<size_t>(to_int(data[p++]));
			std::shared_ptr<rule_start_state> start_state = std::static_pointer_cast<rule_start_state>(atn->states()[state_number]);
			start_state->left_factored(!!to_int(data[p++]));
			std::swap(atn->_rule_start_states[i], start_state);
			if (atn->grammar_type() == atn_type::lexer)
			{
				int32_t token_type = to_int(data[p++]);
				if (token_type == 0xFFFF)
				{
					token_type = token::eof;
				}

				atn->_rule_to_token_type[i] = token_type;

				if (!is_feature_supported(added_lexer_actions, guid))
				{
					// this piece of unused metadata was serialized prior to the
					// addition of LexerAction
					int action_index_ignored = to_int(data[p++]);
					if (action_index_ignored == 0xFFFF)
					{
						action_index_ignored = -1;
					}
				}
			}
		}

		atn->_rule_stop_states.resize(nrules);
		for each (auto state in atn->states())
		{
			if (state->state_type() != atn_state::atn_state_type::rule_stop)
			{
				continue;
			}

			std::shared_ptr<rule_stop_state> stop_state = std::static_pointer_cast<rule_stop_state>(state);
			atn->_rule_stop_states[state->rule_index()] = stop_state;
			atn->_rule_start_states[state->rule_index()]->stop_state(stop_state);
		}

		//
		// MODES
		//
		size_t nmodes = static_cast<size_t>(to_int(data[p++]));
		for (size_t i = 0; i < nmodes; i++)
		{
			size_t state_number = static_cast<size_t>(to_int(data[p++]));
			atn->_mode_start_states.push_back(std::static_pointer_cast<tokens_start_state>(atn->states()[state_number]));
		}

		atn->_mode_dfa.reserve(nmodes);
		for (size_t i = 0; i < nmodes; i++)
		{
			atn->_mode_dfa.push_back(std::make_shared<dynamic_dfa>(atn->_mode_start_states[i]));
		}

		//
		// SETS
		//
		std::vector<interval_set<int32_t>> sets;
		size_t nsets = static_cast<size_t>(to_int(data[p++]));
		for (size_t i = 0; i < nsets; i++)
		{
			size_t nintervals = static_cast<size_t>(to_int(data[p]));
			p++;
			interval_set<int32_t> set;

			bool contains_eof = !!to_int(data[p++]);
			if (contains_eof)
			{
				set.insert(token::eof);
			}

			for (size_t j = 0; j < nintervals; j++)
			{
				int32_t min_value = to_int(data[p]);
				int32_t max_value_inclusive = to_int(data[p + 1]);
				set.insert(std::make_pair(min_value, max_value_inclusive + 1));
				p += 2;
			}

			sets.push_back(std::move(set));
		}

		//
		// EDGES
		//
		size_t nedges = static_cast<size_t>(to_int(data[p++]));
		for (size_t i = 0; i < nedges; i++)
		{
			size_t source_state_number = static_cast<size_t>(to_int(data[p]));
			size_t target_state_number = static_cast<size_t>(to_int(data[p + 1]));
			transition::transition_type transition_type = static_cast<transition::transition_type>(to_int(data[p + 2]));
			int32_t arg1 = to_int(data[p + 3]);
			int32_t arg2 = to_int(data[p + 4]);
			int32_t arg3 = to_int(data[p + 5]);
			std::shared_ptr<transition> transition(edge_factory(atn, transition_type, source_state_number, target_state_number, arg1, arg2, arg3, sets));
			//System.out.println("EDGE "+trans.getClass().getSimpleName()+" "+
			//				   src+"->"+trg+
			//		   " "+Transition.serializationNames[ttype]+
			//		   " "+arg1+","+arg2+","+arg3);
			std::shared_ptr<atn_state> const& source_state = atn->states()[source_state_number];
			source_state->add_transition(std::move(transition));
			p += 6;
		}

		// edges for rule stop states can be derived, so they aren't serialized
		for each (std::shared_ptr<atn_state> state in atn->states())
		{
			bool returning_to_left_factored = state->rule_index() != invalid_rule_index && atn->_rule_start_states[state->rule_index()]->left_factored();
			for (size_t i = 0; i < state->transitions().size(); i++)
			{
				std::shared_ptr<transition> const& t = state->transition(i);
				if (t->type() != transition::transition_type::rule)
				{
					continue;
				}

				std::shared_ptr<rule_transition> rule_transition = std::static_pointer_cast<atn::rule_transition>(t);
				bool returning_from_left_factored = atn->_rule_start_states[rule_transition->target()->rule_index()]->left_factored();
				if (!returning_from_left_factored && returning_to_left_factored)
				{
					continue;
				}

				size_t outermost_precedence_return = ~static_cast<size_t>(0);
				if (atn->_rule_start_states[rule_transition->target()->rule_index()]->precedence_rule())
				{
					if (!rule_transition->precedence())
					{
						outermost_precedence_return = rule_transition->target()->rule_index();
					}
				}

				std::shared_ptr<epsilon_transition> return_transition(std::make_shared<epsilon_transition>(rule_transition->follow_state(), outermost_precedence_return));
				atn->_rule_stop_states[rule_transition->target()->rule_index()]->add_transition(std::move(return_transition));
			}
		}

		for each (std::shared_ptr<atn_state> const& state in atn->states())
		{
			if (is_block_start_state(state))
			{
				std::shared_ptr<block_start_state> block_start = std::static_pointer_cast<block_start_state>(state);

				// we need to know the end state to set its start state
				if (!block_start->end_state())
				{
					throw std::runtime_error("invalid state");
				}

				// block end states can only be associated to a single block start state
				if (block_start->end_state()->start_state())
				{
					throw std::runtime_error("invalid state");
				}

				block_start->end_state()->start_state(block_start);
			}

			if (state->state_type() == atn_state::atn_state_type::plus_loop_back)
			{
				std::shared_ptr<plus_loopback_state> loopback_state = std::static_pointer_cast<plus_loopback_state>(state);
				for (size_t i = 0; i < loopback_state->transitions().size(); i++)
				{
					std::shared_ptr<atn_state> const& target = loopback_state->transition(i)->target();
					if (target->state_type() == atn_state::atn_state_type::plus_block_start)
					{
						std::static_pointer_cast<plus_block_start_state>(target)->loopback_state(loopback_state);
					}
				}
			}
			else if (state->state_type() == atn_state::atn_state_type::star_loop_back)
			{
				std::shared_ptr<star_loopback_state> loopback_state = std::static_pointer_cast<star_loopback_state>(state);
				for (size_t i = 0; i < loopback_state->transitions().size(); i++)
				{
					std::shared_ptr<atn_state> const& target = loopback_state->transition(i)->target();
					if (target->state_type() == atn_state::atn_state_type::star_loop_entry)
					{
						std::static_pointer_cast<star_loop_entry_state>(target)->loopback_state(loopback_state);
					}
				}
			}
		}

		//
		// DECISIONS
		//
		size_t ndecisions = static_cast<size_t>(to_int(data[p++]));
		for (size_t i = 0; i < ndecisions; i++)
		{
			size_t state_number = static_cast<size_t>(to_int(data[p++]));
			std::shared_ptr<decision_state> state = std::static_pointer_cast<decision_state>(atn->states()[state_number]);
			state->decision(i);
			atn->_decisions.push_back(std::move(state));
		}

		//
		// LEXER ACTIONS
		//
		if (atn->grammar_type() == atn_type::lexer)
		{
			if (supports_lexer_actions)
			{
				atn->_lexer_actions.resize(static_cast<size_t>(to_int(data[p++])));
				for (size_t i = 0; i < atn->_lexer_actions.size(); i++)
				{
					lexer_action::lexer_action_type action_type = static_cast<lexer_action::lexer_action_type>(to_int(data[p++]));
					int32_t data1 = to_int(data[p++]);
					if (data1 == 0xFFFF)
					{
						data1 = -1;
					}

					int32_t data2 = to_int(data[p++]);
					if (data2 == 0xFFFF)
					{
						data2 = -1;
					}

					std::shared_ptr<lexer_action> lexer_action = lexer_action_factory(action_type, data1, data2);
					atn->_lexer_actions[i] = std::move(lexer_action);
				}
			}
			else
			{
				// for compatibility with older serialized ATNs, convert the old
				// serialized action index for action transitions to the new
				// form, which is the index of a LexerCustomAction
				std::vector<std::shared_ptr<lexer_action>> legacy_lexer_actions;
				for each (std::shared_ptr<atn_state> const& state in atn->states())
				{
					for (size_t i = 0; i < state->transitions().size(); i++)
					{
						std::shared_ptr<transition> const& transition = state->transition(i);
						if (transition->type() != transition::transition_type::action)
						{
							continue;
						}

						size_t rule_index = std::static_pointer_cast<action_transition>(transition)->rule_index();
						size_t action_index = std::static_pointer_cast<action_transition>(transition)->action_index();
						std::shared_ptr<lexer_action::custom_action> lexer_action(std::make_shared<lexer_action::custom_action>(rule_index, action_index));
						state->set_transition(i, std::make_shared<action_transition>(transition->target(), rule_index, legacy_lexer_actions.size(), false));
						legacy_lexer_actions.push_back(std::move(lexer_action));
					}
				}

				std::move(legacy_lexer_actions.begin(), legacy_lexer_actions.end(), std::back_inserter(atn->_lexer_actions));
			}
		}

		mark_precedence_decisions(atn);

		atn->_decision_dfa.resize(ndecisions);
		for (size_t i = 0; i < ndecisions; i++)
		{
			atn->_decision_dfa[i] = std::make_shared<dynamic_dfa>(atn->_decisions[i], i);
		}

		if (deserialization_options.verify_atn())
		{
			verify_atn(atn);
		}

		if (deserialization_options.generate_rule_bypass_transitions() && atn->grammar_type() == atn_type::parser)
		{
			atn->_rule_to_token_type.resize(atn->_rule_start_states.size());
			for (size_t i = 0; i < atn->_rule_start_states.size(); i++)
			{
				atn->_rule_to_token_type[i] = atn->max_token_type() + static_cast<int32_t>(i) + 1;
			}

			for (size_t i = 0; i < atn->_rule_start_states.size(); i++)
			{
				std::shared_ptr<basic_block_start_state> bypass_start(std::make_shared<basic_block_start_state>(atn, atn->states().size(), i));
				atn->add_state(bypass_start);

				std::shared_ptr<block_end_state> bypass_stop(std::make_shared<block_end_state>(atn, atn->states().size(), i));
				atn->add_state(bypass_stop);

				bypass_start->end_state(bypass_stop);
				atn->define_decision_state(bypass_start);

				bypass_stop->start_state(bypass_start);

				std::shared_ptr<atn_state> end_state;
				std::shared_ptr<transition> exclude_transition;
				if (atn->_rule_start_states[i]->precedence_rule())
				{
					// wrap from the beginning of the rule to the StarLoopEntryState
					for each (std::shared_ptr<atn_state> const& state in atn->states())
					{
						if (state->rule_index() != i)
						{
							continue;
						}

						if (state->state_type() != atn_state::atn_state_type::star_loop_entry)
						{
							continue;
						}

						std::shared_ptr<atn_state> maybe_loop_end_state = state->transition(state->transitions().size() - 1)->target();
						if (maybe_loop_end_state->state_type() != atn_state::atn_state_type::loop_end)
						{
							continue;
						}

						if (maybe_loop_end_state->only_has_epsilon_transitions() && maybe_loop_end_state->transition(0)->target()->state_type() == atn_state::atn_state_type::rule_stop)
						{
							end_state = state;
							break;
						}
					}

					if (!end_state)
					{
						throw std::runtime_error("Couldn't identify final state of the precedence rule prefix section.");
					}

					exclude_transition = std::static_pointer_cast<star_loop_entry_state>(end_state)->loopback_state()->transition(0);
				}
				else
				{
					end_state = atn->_rule_stop_states[i];
				}

				// all non-excluded transitions that currently target end state need to target blockEnd instead
				for each (std::shared_ptr<atn_state> const& state in atn->states())
				{
					for each (std::shared_ptr<transition> const& transition in state->transitions())
					{
						if (transition == exclude_transition)
						{
							continue;
						}

						if (transition->target() == end_state)
						{
							transition->target(bypass_stop);
						}
					}
				}

				// all transitions leaving the rule start state need to leave blockStart instead
				while (!atn->_rule_start_states[i]->transitions().empty())
				{
					std::shared_ptr<transition> transition(atn->_rule_start_states[i]->remove_transition(atn->_rule_start_states[i]->transitions().size() - 1));
					bypass_start->add_transition(std::move(transition));
				}

				// link the new states
				atn->_rule_start_states[i]->add_transition(std::make_shared<epsilon_transition>(bypass_start));
				bypass_stop->add_transition(std::make_shared<epsilon_transition>(end_state));

				std::shared_ptr<atn_state> match_state(std::make_shared<basic_state>(atn, atn->states().size(), i));
				atn->add_state(match_state);
				match_state->add_transition(std::make_shared<atom_transition>(bypass_stop, atn->_rule_to_token_type[i]));
				bypass_start->add_transition(std::make_shared<epsilon_transition>(match_state));
			}

			if (deserialization_options.verify_atn())
			{
				// reverify after modification
				verify_atn(atn);
			}
		}

		if (deserialization_options.optimize())
		{
			for (;;)
			{
				size_t optimization_count = 0;
				optimization_count += inline_set_rules(atn);
				optimization_count += combine_chained_epsilons(atn);
				bool preserve_order = atn->grammar_type() == atn_type::lexer;
				optimization_count += optimize_sets(atn, preserve_order);
				if (!optimization_count)
				{
					break;
				}
			}

			if (deserialization_options.verify_atn())
			{
				// reverify after modification
				verify_atn(atn);
			}
		}

		identify_tail_calls(atn);

		return atn;
	}

}
}
