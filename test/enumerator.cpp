
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <meta/enumerators.hpp>

enum class test { a, b, c, };
static_assert("a" == std::tuple_element_t<0, meta::enumerators<test>>::name);
static_assert("b" == std::tuple_element_t<1, meta::enumerators<test>>::name);
static_assert("c" == std::tuple_element_t<2, meta::enumerators<test>>::name);


namespace xyz {
	enum class test2 { a, b, c, };
}
static_assert("a" == std::tuple_element_t<0, meta::enumerators<xyz::test2>>::name);
static_assert("b" == std::tuple_element_t<1, meta::enumerators<xyz::test2>>::name);
static_assert("c" == std::tuple_element_t<2, meta::enumerators<xyz::test2>>::name);


namespace xyz {
	enum class test3 : unsigned { a, b, c, };
}
static_assert("a" == std::tuple_element_t<0, meta::enumerators<xyz::test3>>::name);
static_assert("b" == std::tuple_element_t<1, meta::enumerators<xyz::test3>>::name);
static_assert("c" == std::tuple_element_t<2, meta::enumerators<xyz::test3>>::name);

namespace xyz {
	struct test4 {
		enum class inner { a, b, c, };
	};
}
static_assert("a" == std::tuple_element_t<0, meta::enumerators<xyz::test4::inner>>::name);
static_assert("b" == std::tuple_element_t<1, meta::enumerators<xyz::test4::inner>>::name);
static_assert("c" == std::tuple_element_t<2, meta::enumerators<xyz::test4::inner>>::name);
