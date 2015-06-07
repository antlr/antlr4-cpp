// Copyright (c) Terence Parr, Sam Harwell. Licensed under the BSD license. See LICENSE in the project root for license information.
#include "stdafx.h"

#include <cassert>

#include "test_interval_set.hpp"

#include <antlr/v4/runtime/token.hpp>
#include <antlr/v4/runtime/misc/interval_set.hpp>

#if defined(_MSC_VER) && (_MSC_VER == 1800)
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) )
#endif

namespace antlr {
namespace test {

	using namespace antlr4;
	using namespace antlr4::misc;

	namespace {

		using interval_set = misc::interval_set<int32_t>;

		interval_set complete_char_set = interval_set::of(std::make_pair(0, 0xFFFE));
		interval_set empty_set;

		std::wstring to_string(interval_set const& set)
		{
			return std::move(misc::to_string<interval_set>()(set));
		}

		void test_single_element()
		{
			interval_set s = interval_set::of(99);
			std::wstring expecting = L"99";
			std::wstring actual(to_string(s));
			assert(actual == expecting);
		}

		void test_min()
		{
			assert(0 == complete_char_set.min());
			assert(token::epsilon == interval_set::combine_or(complete_char_set, interval_set::of(token::epsilon)).min());
			assert(token::eof == interval_set::combine_or(complete_char_set, interval_set::of(token::eof)).min());
		}

		void test_isolated_elements()
		{
			interval_set s;
			s.insert(1);
			s.insert(L'z');
			s.insert(L'\uFFF0');
			std::wstring expecting = L"{1, 122, 65520}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_mixed_ranges_and_elements()
		{
			interval_set s;
			s.insert(1);
			s.insert(std::make_pair(L'a',L'z'+1));
			s.insert(std::make_pair(L'0',L'9'+1));
			std::wstring expecting = L"{1, 48..57, 97..122}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_simple_and()
		{
			interval_set s = interval_set::of(std::make_pair(10,20+1));
			interval_set s2 = interval_set::of(std::make_pair(13,15+1));
			std::wstring expecting = L"{13..15}";
			std::wstring actual = to_string(interval_set::combine_and(s, s2));
			assert(actual == expecting);
		}

		void test_range_and_isolated_element()
		{
			interval_set s = interval_set::of(std::make_pair('a','z'+1));
			interval_set s2 = interval_set::of('d');
			std::wstring expecting = L"100";
			std::wstring actual = to_string(interval_set::combine_and(s, s2));
			assert(actual == expecting);
		}

		void test_empty_intersection()
		{
			interval_set s = interval_set::of(std::make_pair('a','z'+1));
			interval_set s2 = interval_set::of(std::make_pair('0','9'+1));
			std::wstring expecting = L"{}";
			std::wstring actual = to_string(interval_set::combine_and(s, s2));
			assert(actual == expecting);
		}

		void test_empty_intersection_single_elements()
		{
			interval_set s = interval_set::of('a');
			interval_set s2 = interval_set::of('d');
			std::wstring expecting = L"{}";
			std::wstring actual = to_string(interval_set::combine_and(s, s2));
			assert(actual == expecting);
		}

		void test_not_single_element()
		{
			interval_set vocabulary = interval_set::of(std::make_pair(1,1000+1));
			vocabulary.insert(std::make_pair(2000,3000 + 1));
			interval_set s = interval_set::of(std::make_pair(50,50 + 1));
			std::wstring expecting = L"{1..49, 51..1000, 2000..3000}";
			std::wstring actual = to_string(interval_set::complement(s, vocabulary));
			assert(actual == expecting);
		}

		void test_not_set()
		{
			interval_set vocabulary = interval_set::of(std::make_pair(1,1000 + 1));
			interval_set s = interval_set::of(std::make_pair(50,60 + 1));
			s.insert(5);
			s.insert(std::make_pair(250,300 + 1));
			std::wstring expecting = L"{1..4, 6..49, 61..249, 301..1000}";
			std::wstring actual = to_string(interval_set::complement(s, vocabulary));
			assert(actual == expecting);
		}

		void test_not_equal_set()
		{
			interval_set vocabulary = interval_set::of(std::make_pair(1,1000 + 1));
			interval_set s = interval_set::of(std::make_pair(1,1000 + 1));
			std::wstring expecting = L"{}";
			std::wstring actual = to_string(interval_set::complement(s, vocabulary));
			assert(actual == expecting);
		}

		void test_not_set_edge_element()
		{
			interval_set vocabulary = interval_set::of(std::make_pair(1,2 + 1));
			interval_set s = interval_set::of(1);
			std::wstring expecting = L"2";
			std::wstring actual = to_string(interval_set::complement(s, vocabulary));
			assert(actual == expecting);
		}

		void test_not_set_fragmented_vocabulary()
		{
			interval_set vocabulary = interval_set::of(std::make_pair(1,255 + 1));
			vocabulary.insert(std::make_pair(1000,2000 + 1));
			vocabulary.insert(9999);
			interval_set s = interval_set::of(std::make_pair(50, 60 + 1));
			s.insert(3);
			s.insert(std::make_pair(250,300 + 1));
			s.insert(10000); // this is outside range of vocab and should be ignored
			std::wstring expecting = L"{1..2, 4..49, 61..249, 1000..2000, 9999}";
			std::wstring actual = to_string(interval_set::complement(s, vocabulary));
			assert(actual == expecting);
		}

		void test_subtract_of_completely_contained_range()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			interval_set s2 = interval_set::of(std::make_pair(12,15 + 1));
			std::wstring expecting = L"{10..11, 16..20}";
			std::wstring actual = to_string(interval_set::subtract(s, s2));
			assert(actual == expecting);
		}

		void test_subtract_from_set_with_eof()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			s.insert(token::eof);
			interval_set s2 = interval_set::of(std::make_pair(12,15 + 1));
			std::wstring expecting = L"{<EOF>, 10..11, 16..20}";
			std::wstring actual = to_string(interval_set::subtract(s, s2));
			assert(actual == expecting);
		}

		void test_subtract_of_overlapping_range_from_left()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			interval_set s2 = interval_set::of(std::make_pair(5,11 + 1));
			std::wstring expecting = L"{12..20}";
			std::wstring actual = to_string(interval_set::subtract(s, s2));
			assert(actual == expecting);

			interval_set s3 = interval_set::of(std::make_pair(5,10 + 1));
			expecting = L"{11..20}";
			actual = to_string(interval_set::subtract(s, s3));
			assert(actual == expecting);
		}

		void test_subtract_of_overlapping_range_from_right()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			interval_set s2 = interval_set::of(std::make_pair(15,25 + 1));
			std::wstring expecting = L"{10..14}";
			std::wstring actual = to_string(interval_set::subtract(s, s2));
			assert(actual == expecting);

			interval_set s3 = interval_set::of(std::make_pair(20,25 + 1));
			expecting = L"{10..19}";
			actual = to_string(interval_set::subtract(s, s3));
			assert(actual == expecting);
		}

		void test_subtract_of_completely_covered_range()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			interval_set s2 = interval_set::of(std::make_pair(1,25 + 1));
			std::wstring expecting = L"{}";
			std::wstring actual = to_string(interval_set::subtract(s, s2));
			assert(actual == expecting);
		}

		void test_subtract_of_range_spanning_multiple_ranges()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			s.insert(std::make_pair(30,40 + 1));
			s.insert(std::make_pair(50,60 + 1)); // s has 3 ranges now: 10..20, 30..40, 50..60
			interval_set s2 = interval_set::of(std::make_pair(5,55 + 1)); // covers one and touches 2nd range
			std::wstring expecting = L"{56..60}";
			std::wstring actual = to_string(interval_set::subtract(s, s2));
			assert(actual == expecting);

			interval_set s3 = interval_set::of(std::make_pair(15,55 + 1)); // touches both
			expecting = L"{10..14, 56..60}";
			actual = to_string(interval_set::subtract(s, s3));
			assert(actual == expecting);
		}

		/** The following was broken:
			{0..113, 115..65534}-{0..115, 117..65534}=116..65534
		 */
		void test_subtract_of_wacky_range()
		{
			interval_set s = interval_set::of(std::make_pair(0,113 + 1));
			s.insert(std::make_pair(115,200 + 1));
			interval_set s2 = interval_set::of(std::make_pair(0,115 + 1));
			s2.insert(std::make_pair(117,200 + 1));
			std::wstring expecting = L"116";
			std::wstring actual = to_string(interval_set::subtract(s, s2));
			assert(actual == expecting);
		}

		void test_simple_equals()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			interval_set s2 = interval_set::of(std::make_pair(10,20 + 1));
			assert(s == s2);

			interval_set s3 = interval_set::of(std::make_pair(15,55 + 1));
			assert(!(s == s3));
		}

		void test_equals()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			s.insert(2);
			s.insert(std::make_pair(499,501 + 1));
			interval_set s2 = interval_set::of(std::make_pair(10,20 + 1));
			s2.insert(2);
			s2.insert(std::make_pair(499,501 + 1));
			assert(s == s2);

			interval_set s3 = interval_set::of(std::make_pair(10,20 + 1));
			s3.insert(2);
			assert(!(s == s3));
		}

		void test_single_element_minus_disjoint_set()
		{
			interval_set s = interval_set::of(std::make_pair(15,15 + 1));
			interval_set s2 = interval_set::of(std::make_pair(1,5 + 1));
			s2.insert(std::make_pair(10,20 + 1));
			std::wstring expecting = L"{}"; // 15 - {1..5, 10..20} = {}
			std::wstring actual = to_string(interval_set::subtract(s, s2));
			assert(actual == expecting);
		}

		void test_membership()
		{
			interval_set s = interval_set::of(std::make_pair(15,15 + 1));
			s.insert(std::make_pair(50,60 + 1));
			assert(!s.contains(0));
			assert(!s.contains(20));
			assert(!s.contains(100));
			assert(s.contains(15));
			assert(s.contains(55));
			assert(s.contains(50));
			assert(s.contains(60));
		}

		// {2,15,18} & 10..20
		void test_intersection_with_two_contained_elements()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			interval_set s2 = interval_set::of(std::make_pair(2,2 + 1));
			s2.insert(15);
			s2.insert(18);
			std::wstring expecting = L"{15, 18}";
			std::wstring actual = to_string(interval_set::combine_and(s, s2));
			assert(actual == expecting);
		}

		void test_intersection_with_two_contained_elements_reversed()
		{
			interval_set s = interval_set::of(std::make_pair(10,20 + 1));
			interval_set s2 = interval_set::of(std::make_pair(2,2 + 1));
			s2.insert(15);
			s2.insert(18);
			std::wstring expecting = L"{15, 18}";
			std::wstring actual = to_string(interval_set::combine_and(s2, s));
			assert(actual == expecting);
		}

		void test_complement()
		{
			interval_set s = interval_set::of(std::make_pair(100,100 + 1));
			s.insert(std::make_pair(101, 101 + 1));
			interval_set s2 = interval_set::of(std::make_pair(100,102 + 1));
			std::wstring expecting = L"102";
			std::wstring actual = to_string(interval_set::complement(s, s2));
			assert(actual == expecting);
		}

		void test_complement2()
		{
			interval_set s = interval_set::of(std::make_pair(100,101 + 1));
			interval_set s2 = interval_set::of(std::make_pair(100,102 + 1));
			std::wstring expecting = L"102";
			std::wstring actual = to_string(interval_set::complement(s, s2));
			assert(actual == expecting);
		}

		void test_complement3()
		{
			interval_set s = interval_set::of(std::make_pair(1,96 + 1));
			s.insert(std::make_pair(99, L'\uFFFE' + 1));
			std::wstring expecting = L"{97..98}";
			std::wstring actual = to_string(interval_set::complement(s, std::make_pair(1, L'\uFFFE' + 1)));
			assert(actual == expecting);
		}

		void test_merge_of_ranges_and_single_values()
		{
			// {0..41, 42, 43..65534}
			interval_set s = interval_set::of(std::make_pair(0,41 + 1));
			s.insert(42);
			s.insert(std::make_pair(43, 65534 + 1));
			std::wstring expecting = L"{0..65534}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_merge_of_ranges_and_single_values_reverse()
		{
			interval_set s = interval_set::of(std::make_pair(43,65534 + 1));
			s.insert(42);
			s.insert(std::make_pair(0, 41 + 1));
			std::wstring expecting = L"{0..65534}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_merge_where_addition_merges_two_existing_intervals()
		{
			// 42, 10, {0..9, 11..41, 43..65534}
			interval_set s = interval_set::of(42);
			s.insert(10);
			s.insert(std::make_pair(0, 9 + 1));
			s.insert(std::make_pair(43, 65534 + 1));
			s.insert(std::make_pair(11, 41 + 1));
			std::wstring expecting = L"{0..65534}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		/**
		 * This case is responsible for antlr/antlr4#153.
		 * https://github.com/antlr/antlr4/issues/153
		 */
		void test_merge_where_addition_merges_three_existing_intervals()
		{
			interval_set s;
			s.insert(0);
			s.insert(3);
			s.insert(5);
			s.insert(std::make_pair(0, 7 + 1));
			std::wstring expecting = L"{0..7}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_merge_with_double_overlap()
		{
			interval_set s = interval_set::of(std::make_pair(1,10 + 1));
			s.insert(std::make_pair(20, 30 + 1));
			s.insert(std::make_pair(5, 25 + 1)); // overlaps two!
			std::wstring expecting = L"{1..30}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_size()
		{
			interval_set s = interval_set::of(std::make_pair(20,30 + 1));
			s.insert(std::make_pair(50, 55 + 1));
			s.insert(std::make_pair(5, 19 + 1));
			int32_t expecting = 32;
			int32_t actual = s.size();
			assert(actual == expecting);
		}

		//void test_to_list() {
		//	interval_set s = interval_set::of(std::make_pair(20, 25));
		//	s.insert(std::make_pair(50, 55));
		//	s.insert(std::make_pair(5, 5));
		//	std::wstring expecting = L"[5, 20, 21, 22, 23, 24, 25, 50, 51, 52, 53, 54, 55]";
		//	std::wstring actual = std::wstring.valueOf(s.toList());
		//	assert(actual == expecting);
		//}

		/** The following was broken:
			{'\u0000'..'s', 'u'..'\uFFFE'} & {'\u0000'..'q', 's'..'\uFFFE'}=
			{'\u0000'..'q', 's'}!!!! broken...
			'q' is 113 ascii
			'u' is 117
		*/
		void test_not_r_intersection_not_t()
		{
			interval_set s = interval_set::of(std::make_pair(0,'s' + 1));
			s.insert(std::make_pair('u',200 + 1));
			interval_set s2 = interval_set::of(std::make_pair(0,'q' + 1));
			s2.insert(std::make_pair('s',200 + 1));
			std::wstring expecting = L"{0..113, 115, 117..200}";
			std::wstring actual = to_string(interval_set::combine_and(s, s2));
			assert(actual == expecting);
		}

		void test_remove_single_element()
		{
			interval_set s = interval_set::of(std::make_pair(1,10 + 1));
			s.insert(std::make_pair(-3,-3 + 1));
			s.remove(-3);
			std::wstring expecting = L"{1..10}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_remove_left_side()
		{
			interval_set s = interval_set::of(std::make_pair(1,10 + 1));
			s.insert(std::make_pair(-3,-3 + 1));
			s.remove(1);
			std::wstring expecting = L"{-3, 2..10}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_remove_right_side()
		{
			interval_set s = interval_set::of(std::make_pair(1,10 + 1));
			s.insert(std::make_pair(-3,-3 + 1));
			s.remove(10);
			std::wstring expecting = L"{-3, 1..9}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

		void test_remove_middle_range()
		{
			interval_set s = interval_set::of(std::make_pair(1,10 + 1));
			s.insert(std::make_pair(-3,-3 + 1));
			s.remove(5);
			std::wstring expecting = L"{-3, 1..4, 6..10}";
			std::wstring actual = to_string(s);
			assert(actual == expecting);
		}

	}

	void test_interval_set()
	{
		test_single_element();
		test_min();
		test_isolated_elements();
		test_mixed_ranges_and_elements();
		test_simple_and();
		test_range_and_isolated_element();
		test_empty_intersection();
		test_empty_intersection_single_elements();
		test_not_single_element();
		test_not_set();
		test_not_equal_set();
		test_not_set_edge_element();
		test_not_set_fragmented_vocabulary();
		test_subtract_of_completely_contained_range();
		test_subtract_from_set_with_eof();
		test_subtract_of_overlapping_range_from_left();
		test_subtract_of_overlapping_range_from_right();
		test_subtract_of_completely_covered_range();
		test_subtract_of_range_spanning_multiple_ranges();
		test_subtract_of_wacky_range();
		test_simple_equals();
		test_equals();
		test_single_element_minus_disjoint_set();
		test_membership();
		test_intersection_with_two_contained_elements();
		test_intersection_with_two_contained_elements_reversed();
		test_complement();
		test_complement2();
		test_complement3();
		test_merge_of_ranges_and_single_values();
		test_merge_of_ranges_and_single_values_reverse();
		test_merge_where_addition_merges_two_existing_intervals();
		test_merge_where_addition_merges_three_existing_intervals();
		test_merge_with_double_overlap();
		test_size();
		//test_to_list();
		test_not_r_intersection_not_t();
		test_remove_single_element();
		test_remove_left_side();
		test_remove_right_side();
		test_remove_middle_range();
	}

}
}
