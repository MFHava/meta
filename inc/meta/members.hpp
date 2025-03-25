
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef METADATA_MAX_AGGREGATE_MEMBERS
	#error "Missing METADATA_MAX_AGGREGATE_MEMBERS"
#endif

#include <tuple>
#include <type_traits>
#include <boost/preprocessor.hpp>
#include "fixed_string.hpp"

namespace meta::internal {
	template<typename T>
	struct any_base final { //used to check that T has no base...
		template<typename U>
		requires(std::is_base_of_v<U, T> and not std::is_same_v<T, U>)
		operator U();
	};
}

namespace meta {
	//! @brief aggregate without base-classes
	template<typename T>
	concept simple_aggregate = std::is_aggregate_v<T> and std::is_default_constructible_v<T> /*TODO: use this instead? std::default_initializable<std::remove_const_t<T>> */ and not requires { T{internal::any_base<T>{}}; } /*== has no base class*/;
}

namespace meta::internal {
	template<typename T>
	concept aggregate_0 = simple_aggregate<T> and requires { T{}; };
	constexpr
	auto as_tuple(aggregate_0 auto &) noexcept { return std::tuple<>{}; }

	#define METADATA_INTERNAL_INIT(Z, N, _) {}
	#define METADATA_INTERNAL_BINDING(Z, N, _) BOOST_PP_CAT(_, N)
	#define METADATA_INTERNAL_AS_TUPLE(Z, N, _)\
		template<typename T>\
		concept BOOST_PP_CAT(aggregate_, N) = BOOST_PP_CAT(aggregate_, BOOST_PP_SUB(N, 1))<T> and requires { T{ BOOST_PP_ENUM(N, METADATA_INTERNAL_INIT, _) }; };\
		constexpr\
		auto as_tuple(BOOST_PP_CAT(aggregate_, N) auto & val) noexcept {\
			auto & [BOOST_PP_ENUM(N, METADATA_INTERNAL_BINDING, _)]{val};\
			return std::tie(BOOST_PP_ENUM(N, METADATA_INTERNAL_BINDING, _));\
		}
	BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_ADD(METADATA_MAX_AGGREGATE_MEMBERS, 1), METADATA_INTERNAL_AS_TUPLE, _)
	#undef METADATA_INTERNAL_AS_TUPLE

	//overflow detection:
	template<typename T>
	concept BOOST_PP_CAT(aggregate_, BOOST_PP_ADD(METADATA_MAX_AGGREGATE_MEMBERS, 1)) = BOOST_PP_CAT(aggregate_, METADATA_MAX_AGGREGATE_MEMBERS)<T> and requires { T{ BOOST_PP_ENUM(BOOST_PP_ADD(METADATA_MAX_AGGREGATE_MEMBERS, 1), METADATA_INTERNAL_INIT, _) }; };
	constexpr
	auto as_tuple(BOOST_PP_CAT(aggregate_, BOOST_PP_ADD(METADATA_MAX_AGGREGATE_MEMBERS, 1)) auto &) noexcept =delete; //detected simple_aggregate with more members than supported

	#undef METADATA_INTERNAL_BINDING
	#undef METADATA_INTERNAL_INIT

	template<auto P>
	consteval
	auto compute_member_name() noexcept {
#if defined(_MSC_VER)
		constexpr std::string_view func{__FUNCSIG__};
#elif defined(__GNUC__) || defined(__clang__)
		constexpr std::string_view func{__PRETTY_FUNCTION__};
#else
	#error "Unsupported compiler"
#endif
		constexpr auto name{[](std::string_view decorated) {
#if defined(__clang__)
			decorated.remove_suffix(decorated.size() - decorated.rfind('}'));
			decorated.remove_prefix(decorated.rfind('.') + 1);
#elif defined(__GNUC__)
			decorated.remove_prefix(decorated.rfind(':') + 1);
			decorated.remove_suffix(decorated.size() - decorated.rfind(')'));
#elif defined(_MSC_VER)
			decorated.remove_suffix(decorated.size() - decorated.rfind('>'));
			decorated.remove_prefix(decorated.rfind('>') + 1);
#endif
			return decorated;
		}(func)};
		return fixed_string<name.size()>{name};
	}


#ifdef __clang__
	//need to add workaround to get necessary info from Clang...
	template<typename T>
	struct clang_workaround final { T val; };

	#define META_INTERNAL_CLANG_WORKAROUND(expr) internal::clang_workaround<decltype(expr)>{expr}

	//also need to silence clang on missing definition of fake...
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wundefined-var-template"
#else
	#define META_INTERNAL_CLANG_WORKAROUND(expr) expr
#endif


	template<simple_aggregate T>
	extern
	const T fake; //pretending there is an object somewhere...
}

namespace meta {
	template<simple_aggregate T, std::size_t Index>
	struct member final {
		static
		constexpr
		auto access(T & self) noexcept -> decltype(auto) { return (std::get<Index>(internal::as_tuple(self))); }
		static
		constexpr
		auto access(const T & self) noexcept -> decltype(auto) { return (std::get<Index>(internal::as_tuple(self))); }

		using type = std::remove_cvref_t<decltype(access(internal::fake<T>))>;

		inline
		static
		constexpr
		fixed_string name{internal::compute_member_name<META_INTERNAL_CLANG_WORKAROUND(std::addressof(access(internal::fake<T>)))>()};
	};


	template<simple_aggregate T>
	using members = decltype([]<auto... Indices>(std::index_sequence<Indices...>) { return std::make_tuple(member<T, Indices>{}...); }(std::make_index_sequence<std::tuple_size_v<decltype(internal::as_tuple(internal::fake<T>))>>{}));
}

#undef META_INTERNAL_CLANG_WORKAROUND

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

namespace meta {
	//! @brief user-friendly API to iterate all members of an aggregate
	inline
	constexpr
	class { //niebloid
		constexpr
		void do_(internal::aggregate_0 auto &, auto) const noexcept { /*nop*/ }

		template<internal::aggregate_1 T>
		constexpr
		void do_(T & self, auto func) const {
			using Members = members<std::decay_t<T>>;
			constexpr auto Size{std::tuple_size_v<Members>};
			using FirstMember = std::tuple_element_t<0, Members>;

			if constexpr(requires { func(FirstMember{}, self); }) {
				[&]<auto... I>(std::index_sequence<I...>) {
					(func(std::tuple_element_t<I, Members>{}, self), ...);
				}(std::make_index_sequence<Size>{});
			} else {
				static_assert(requires { func(FirstMember::access(self)); });
				[&]<auto... I>(std::index_sequence<I...>) {
					(func(std::tuple_element_t<I, Members>::access(self)), ...);
				}(std::make_index_sequence<Size>{});
			}
		}
	public:
		template<simple_aggregate Self>
		constexpr
		void operator()(
			Self & self, //!< [in] object to invoke @c func on for every member
			auto func    //!< [in] must support either @c func(m) where @c m is a reference to a member of @c self or @c func(M, self) where @c M is @c member<Self, I>
		) const { do_(self, func); }
	} for_each_member;
}

