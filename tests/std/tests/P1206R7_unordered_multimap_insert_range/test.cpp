// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <ranges>
#include <span>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

static constexpr pair<int, int> expected[] = {
    {0, 20}, {1, 19}, {2, 18}, {3, 17}, {4, 16}, {4, 15}, {4, 14}, {5, 13}, {5, 13}, {6, 12}, {7, 11}};

template <ranges::input_range Rng>
void test_unordered_multimap(Rng&& rng) {
    if constexpr (is_reference_v<ranges::range_reference_t<Rng>>) {
        unordered_multimap<int, int> m;
        m.insert_range(forward<Rng>(rng));
        unordered_multimap<int, int> e{begin(expected), end(expected)};
        assert(m == e);
    }
}

static constexpr pair<int, int> some_pairs[] = {
    {4, 16}, {2, 18}, {3, 17}, {0, 20}, {4, 15}, {5, 13}, {6, 12}, {4, 14}, {5, 13}, {7, 11}, {1, 19}};

struct unordered_multimap_instantiator {
    template <ranges::input_range R>
    static void call() {
        test_unordered_multimap(R{some_pairs});
    }
};

template <class Category, test::Common IsCommon, bool is_random = derived_from<Category, random_access_iterator_tag>>
using move_only_view = test::range<Category, const pair<int, int>, test::Sized{is_random},
    test::CanDifference{is_random}, IsCommon, test::CanCompare{derived_from<Category, forward_iterator_tag>},
    test::ProxyRef::no, test::CanView::yes, test::Copyability::move_only>;

void test_copyable_views() {
    constexpr span<const pair<int, int>> s{some_pairs};
    test_unordered_multimap(s);
}

void test_move_only_views() {
    test_unordered_multimap(move_only_view<input_iterator_tag, test::Common::no>{some_pairs});
    test_unordered_multimap(move_only_view<forward_iterator_tag, test::Common::no>{some_pairs});
    test_unordered_multimap(move_only_view<forward_iterator_tag, test::Common::yes>{some_pairs});
    test_unordered_multimap(move_only_view<bidirectional_iterator_tag, test::Common::no>{some_pairs});
    test_unordered_multimap(move_only_view<bidirectional_iterator_tag, test::Common::yes>{some_pairs});
    test_unordered_multimap(move_only_view<random_access_iterator_tag, test::Common::no>{some_pairs});
    test_unordered_multimap(move_only_view<random_access_iterator_tag, test::Common::yes>{some_pairs});
}

void test_c_array() {
    test_unordered_multimap(some_pairs);
}

void test_lvalue_vector() {
    vector vec(ranges::begin(some_pairs), ranges::end(some_pairs));
    test_unordered_multimap(vec);
}

void test_lvalue_forward_list() {
    forward_list lst(ranges::begin(some_pairs), ranges::end(some_pairs));
    test_unordered_multimap(lst);
}

int main() {
    // Validate views
    test_copyable_views();
    test_move_only_views();

    // Validate non-views
    test_c_array();
    test_lvalue_vector();
    test_lvalue_forward_list();

    test_in<unordered_multimap_instantiator, const pair<int, int>>();
}
