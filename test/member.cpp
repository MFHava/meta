
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <meta/members.hpp>

struct test final {
	int a;
	bool b;
	std::string c;
};
static_assert("a" == std::tuple_element_t<0, meta::members<test>>::name);
static_assert("b" == std::tuple_element_t<1, meta::members<test>>::name);
static_assert("c" == std::tuple_element_t<2, meta::members<test>>::name);

template<typename T>
struct test2 final {
	int a;
	bool b;
	std::string c;
};
static_assert("a" == std::tuple_element_t<0, meta::members<test2<int>>>::name);
static_assert("b" == std::tuple_element_t<1, meta::members<test2<int>>>::name);
static_assert("c" == std::tuple_element_t<2, meta::members<test2<int>>>::name);

namespace xyz {
	template<typename T>
	struct test3 final {
		int a;
		bool b;
		std::string c;
	};
}
static_assert("a" == std::tuple_element_t<0, meta::members<xyz::test3<int>>>::name);
static_assert("b" == std::tuple_element_t<1, meta::members<xyz::test3<int>>>::name);
static_assert("c" == std::tuple_element_t<2, meta::members<xyz::test3<int>>>::name);

namespace xyz {
	struct test4 final {
		struct inner {
			int a;
			bool b;
			std::string c;
		};
	};
}
static_assert("a" == std::tuple_element_t<0, meta::members<xyz::test4::inner>>::name);
static_assert("b" == std::tuple_element_t<1, meta::members<xyz::test4::inner>>::name);
static_assert("c" == std::tuple_element_t<2, meta::members<xyz::test4::inner>>::name);

namespace xyz {
	template<typename T>
	struct test5 final {
		template<typename U>
		struct inner {
			int a;
			bool b;
			std::string c;
		};
	};
}
static_assert("a" == std::tuple_element_t<0, meta::members<xyz::test5<int>::inner<double>>>::name);
static_assert("b" == std::tuple_element_t<1, meta::members<xyz::test5<int>::inner<double>>>::name);
static_assert("c" == std::tuple_element_t<2, meta::members<xyz::test5<int>::inner<double>>>::name);

int main() {}
