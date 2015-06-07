// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#pragma once

#include <cstdint>
#include <algorithm>
#include <sstream>
#include <vector>

#include "../token.hpp"
#include "to_string.hpp"

namespace antlr4 {
namespace misc {

	template<typename _Ty = int32_t, typename _Alloc = std::allocator<std::pair<_Ty, _Ty>>>
	class interval_set
	{
		static_assert(std::is_integral<_Ty>::value, "interval_set only works with integers");

	public:
		typedef _Ty value_type;
		typedef std::pair<_Ty, _Ty> interval_type;

	private:
		std::vector<interval_type, _Alloc> _pairs;

	public:
		interval_set()
		{
		}

		interval_set(interval_set const& set)
			: _pairs(set._pairs)
		{
		}

		interval_set(interval_set&& set)
			: _pairs(std::move(set.pairs()))
		{
		}

		interval_set& operator= (interval_set const& set)
		{
			_pairs = set.pairs();
		}

	public:
		static interval_set of(_Ty value)
		{
			interval_set result;
			result.insert(value);
			return std::move(result);
		}

		static interval_set of(interval_type interval)
		{
			interval_set result;
			result.insert(interval);
			return std::move(result);
		}

	public:
		_Ty min() const
		{
			if (_pairs.empty())
			{
				return 0;
			}

			return _pairs[0].first;
		}

		_Ty max() const
		{
			if (_pairs.empty())
			{
				return 0;
			}

			return _pairs.back().second - 1;
		}

		std::vector<interval_type> const& pairs() const
		{
			return _pairs;
		}

		bool empty() const
		{
			return _pairs.empty();
		}

		_Ty size() const
		{
			_Ty result = _Ty();
			for each (interval_type const& pair in pairs())
			{
				result += (pair.second - pair.first);
			}

			return result;
		}

		bool contains(_Ty value) const
		{
			// this could be greatly improved with a binary search
			for each (interval_type const& pair in pairs())
			{
				if (value < pair.first)
				{
					// list is sorted and value is before this interval
					return false;
				}

				// NOTE: duplicate range check omitted
				if (/*value >= pair.first &&*/ value <= pair.second)
				{
					// found in this interval
					return true;
				}
			}

			return false;
		}

	public:
		void insert(_Ty value)
		{
			insert(std::make_pair(value, value + 1));
		}

		void insert(interval_type range)
		{
			if (range.second <= range.first)
			{
				return;
			}

			for (auto iterator = _pairs.begin(); iterator != _pairs.end(); ++iterator)
			{
				interval_type pair = *iterator;
				if (range.first == pair.first && range.second == pair.second)
				{
					return;
				}

				bool mergeable = (range.first <= pair.second) && (range.second >= pair.first);
				if (mergeable)
				{
					// next to each other, make a single larger interval
					interval_type merged = std::make_pair(std::min(range.first, pair.first), std::max(range.second, pair.second));
					*iterator = merged;

					// make sure we didn't just create an interval that should be merged with next interval in list
					while (++iterator != _pairs.end())
					{
						pair = *iterator;
						mergeable = (merged.first <= pair.second) && (merged.second >= pair.first);
						if (!mergeable)
						{
							break;
						}

						// if we bump up against or overlap next, merge

						// remove this one
						iterator = _pairs.erase(iterator);
						// move backwards one element
						--iterator;
						// update to the union
						*iterator = std::make_pair(std::min(iterator->first, pair.first), std::max(iterator->second, pair.second));
					}

					return;
				}
				else if (range.second < pair.first)
				{
					// insert before the current position
					_pairs.insert(iterator, range);
					return;
				}

				// if disjoint and after `pair`, a future iteration will handle it
			}

			// must be after last interval (and disjoint from last interval). just add it.
			_pairs.push_back(range);
		}

		void insert(interval_set const& set)
		{
			if (this == &set)
			{
				return;
			}

			for each (interval_type pair in set.pairs())
			{
				insert(pair);
			}
		}

		void remove(_Ty value)
		{
			size_t n = pairs().size();
			for (size_t i = 0; i < n; i++)
			{
				interval_type interval(_pairs[i]);
				_Ty a = interval.first;
				_Ty b = interval.second;
				if (value < a)
				{
					// list is sorted and el is before this interval; not here
					break;
				}

				if (value == a && value + 1 == b)
				{
					// if whole interval x..x, rm
					_pairs.erase(_pairs.begin() + static_cast<ptrdiff_t>(i));
					break;
				}

				// if on left edge x..b, adjust left
				if (value == a)
				{
					_pairs[i] = std::make_pair(interval.first + 1, interval.second);
					break;
				}

				// if on right edge a..x, adjust right
				if (value + 1 == b)
				{
					_pairs[i] = std::make_pair(interval.first, interval.second - 1);
					break;
				}

				// if in middle a..x..b, split interval
				if (value > a && value + 1 < b)
				{
					// found in this interval
					_Ty old_b = interval.second;
					_pairs[i] = std::make_pair(interval.first, value);
					insert(std::make_pair(value + 1, old_b));
				}
			}
		}

	public:
		static interval_set combine_or(interval_set const& x, interval_set const& y)
		{
			interval_set result(x);
			result.insert(y);
			return std::move(result);
		}

		static interval_set combine_and(interval_set const& x, interval_set const& y)
		{

			std::vector<interval_type> const& my_intervals = x.pairs();
			std::vector<interval_type> const& their_intervals = y.pairs();
			interval_set result;
			size_t my_size = my_intervals.size();
			size_t their_size = their_intervals.size();
			size_t i = 0;
			size_t j = 0;
			while (i < my_size && j < their_size)
			{
				interval_type mine = my_intervals[i];
				interval_type theirs = their_intervals[j];
				if (mine.second <= theirs.first)
				{
					// move this iterator looking for interval that might overlap
					i++;
				}
				else if (theirs.second <= mine.first)
				{
					// move other iterator looking for interval that might overlap
					j++;
				}
				else if (mine.first <= theirs.first && mine.second >= theirs.second)
				{
					// overlap, add intersection, get next theirs
					result.insert(theirs);
					j++;
				}
				else if (theirs.first <= mine.first && theirs.second >= mine.second)
				{
					// overlap, add intersection, get next mine
					result.insert(mine);
					i++;
				}
				else if (mine.first < theirs.second && mine.second > theirs.first)
				{
					result.insert(std::make_pair(std::max(mine.first, theirs.first), std::min(mine.second, theirs.second)));

					// Move the iterator of lower range [a..b], but not
					// the upper range as it may contain elements that will collide
					// with the next iterator. So, if mine=[0..115] and
					// theirs=[115..200], then intersection is 115 and move mine
					// but not theirs as theirs may collide with the next range
					// in thisIter.
					// move both iterators to next ranges
					if (mine.first > theirs.first) {
						j++;
					}
					else if (theirs.first > mine.first) {
						i++;
					}
				}
			}

			return std::move(result);
		}

		static interval_set complement(interval_set const& set, interval_set const& vocabulary)
		{
			interval_set result(vocabulary);
			if (result.empty())
			{
				return std::move(result);
			}

			return std::move(interval_set::subtract(result, set));
		}

		static interval_set complement(interval_set const& set, interval_type vocabulary)
		{
			return std::move(complement(set, interval_set::of(vocabulary)));
		}

		static interval_set subtract(interval_set const& left, interval_set const& right)
		{
			if (left.empty())
			{
				return left;
			}

			if (right.empty())
			{
				return left;
			}

			interval_set result(left);
			size_t result_index = 0;
			size_t right_index = 0;
			while (result_index < result.pairs().size() && right_index < right.pairs().size())
			{
				interval_type result_interval = result.pairs()[result_index];
				interval_type right_interval = right.pairs()[right_index];

				// operation: (result_interval - right_interval) and update indexes

				if (right_interval.second <= result_interval.first)
				{
					right_index++;
					continue;
				}

				if (right_interval.first >= result_interval.second)
				{
					result_index++;
					continue;
				}

				interval_type before_current;
				interval_type after_current;
				if (right_interval.first > result_interval.first)
				{
					before_current = interval_type(result_interval.first, right_interval.first);
				}

				if (right_interval.second < result_interval.second)
				{
					after_current = interval_type(right_interval.second, result_interval.second);
				}

				if (before_current.second > before_current.first)
				{
					if (after_current.second > after_current.first)
					{
						// split the current interval into two
						result._pairs[result_index] = before_current;
						result._pairs.insert(result._pairs.begin() + static_cast<ptrdiff_t>(result_index) + 1, after_current);
						result_index++;
						right_index++;
						continue;
					}
					else
					{
						// replace the current interval
						result._pairs[result_index] = before_current;
						result_index++;
						continue;
					}
				}
				else
				{
					if (after_current.second > after_current.first)
					{
						// replace the current interval
						result._pairs[result_index] = after_current;
						right_index++;
						continue;
					}
					else
					{
						// remove the current interval (thus no need to increment resultI)
						result._pairs.erase(result._pairs.begin() + static_cast<ptrdiff_t>(result_index));
						continue;
					}
				}
			}

			// If right_index reached right.pairs().size(), no more intervals to subtract from result.
			// If result_index reached result.pairs().size(), we would be subtracting from an empty set.
			// Either way, we are done.
			return std::move(result);
		}
	};

	template<typename _Ty, typename _AllocX, typename _AllocY>
	bool operator== (interval_set<_Ty, _AllocX> const& x, interval_set<_Ty, _AllocY> const& y)
	{
		return x.pairs() == y.pairs();
	}

	template<>
	struct to_string<interval_set<int32_t>>
	{
		std::wstring operator() (interval_set<int32_t> const& set, bool elements_are_characters) const
		{
			if (set.pairs().empty())
			{
				return L"{}";
			}

			std::wostringstream stream;
			if (set.size() > 1)
			{
				stream << L"{";
			}

			bool first = true;
			for each (interval_set<int32_t>::interval_type const& interval in set.pairs())
			{
				if (first)
				{
					first = false;
				}
				else
				{
					stream << L", ";
				}

				if (interval.first == (interval.second - 1))
				{
					if (interval.first == token::eof)
					{
						stream << L"<EOF>";
					}
					else if (elements_are_characters)
					{
						stream << L"'" << static_cast<wchar_t>(interval.first) << L"'";
					}
					else
					{
						stream << interval.first;
					}
				}
				else
				{
					if (elements_are_characters)
					{
						stream << L"'" << static_cast<wchar_t>(interval.first) << L"'..'" << static_cast<wchar_t>(interval.second - 1) << L"'";
					}
					else
					{
						stream << interval.first << L".." << (interval.second - 1);
					}
				}
			}

			if (set.size() > 1)
			{
				stream << L"}";
			}

			return stream.str();
		}

		std::wstring operator() (interval_set<int32_t> const& set) const
		{
			return (*this)(set, false);
		}
	};

}
}
