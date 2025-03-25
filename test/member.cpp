
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
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


TEST_CASE("for_each_member empty", "[members]") {
	struct empty {};
	empty e;

	auto invoked{false};
	meta::for_each_member(e, [&](auto &) { invoked = true; });
	REQUIRE(!invoked);

	meta::for_each_member(e, [&](auto, auto &) { invoked = true; });
	REQUIRE(!invoked);

	meta::for_each_member(e, [&]<typename M>(M, auto &) { invoked = true; });
	REQUIRE(!invoked);
}

TEST_CASE("for_each_member const non-empty", "[members]") {
	const test t{12, false, "hello"};

	std::size_t count{0};
	meta::for_each_member(t, [&](auto &) { ++count; });
	REQUIRE(count == 3);

	meta::for_each_member(t, [&](auto, auto &) { ++count; });
	REQUIRE(count == 6);

	meta::for_each_member(t, [&]<typename M>(M, auto &) { ++count; });
	REQUIRE(count == 9);
}

TEST_CASE("for_each_member mutable non-empty", "[members]") {
	test t{12, false, "hello"};

	std::size_t count{0};
	meta::for_each_member(t, [&](auto &) { ++count; });
	REQUIRE(count == 3);

	meta::for_each_member(t, [&](auto, auto &) { ++count; });
	REQUIRE(count == 6);

	meta::for_each_member(t, [&]<typename M>(M, auto &) { ++count; });
	REQUIRE(count == 9);
}

