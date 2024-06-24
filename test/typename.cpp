
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <meta/typename.hpp>

struct X;
enum Y : int;
enum class Z;
union W;

struct T {
	struct X;
	enum Y : int;
	enum class Z;
	union W;


	struct U {
		struct X;
		enum Y : int;
		enum class Z;
		union W;
	};
};


static_assert(meta::type_name<X> == "X");
static_assert(std::tuple_size_v<decltype(meta::scopes<X>)> == 0);

static_assert(meta::type_name<Y> == "Y");
static_assert(std::tuple_size_v<decltype(meta::scopes<Y>)> == 0);

static_assert(meta::type_name<Z> == "Z");
static_assert(std::tuple_size_v<decltype(meta::scopes<Z>)> == 0);

static_assert(meta::type_name<W> == "W");
static_assert(std::tuple_size_v<decltype(meta::scopes<W>)> == 0);


static_assert(meta::type_name<T::X> == "X");
static_assert(std::get<0>(meta::scopes<T::X>) == "T");

static_assert(meta::type_name<T::Y> == "Y");
static_assert(std::get<0>(meta::scopes<T::Y>) == "T");

static_assert(meta::type_name<T::Z> == "Z");
static_assert(std::get<0>(meta::scopes<T::Z>) == "T");

static_assert(meta::type_name<T::W> == "W");
static_assert(std::get<0>(meta::scopes<T::W>) == "T");


static_assert(meta::type_name<T::U::X> == "X");
static_assert(std::get<0>(meta::scopes<T::U::X>) == "T");
static_assert(std::get<1>(meta::scopes<T::U::X>) == "U");

static_assert(meta::type_name<T::U::Y> == "Y");
static_assert(std::get<0>(meta::scopes<T::U::Y>) == "T");
static_assert(std::get<1>(meta::scopes<T::U::Y>) == "U");

static_assert(meta::type_name<T::U::Z> == "Z");
static_assert(std::get<0>(meta::scopes<T::U::Z>) == "T");
static_assert(std::get<1>(meta::scopes<T::U::Z>) == "U");

static_assert(meta::type_name<T::U::W> == "W");
static_assert(std::get<0>(meta::scopes<T::U::W>) == "T");
static_assert(std::get<1>(meta::scopes<T::U::W>) == "U");
