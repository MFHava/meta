
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <array>
#include <tuple>
#include <utility>
#include <type_traits>
#include "fixed_string.hpp"

namespace meta {
	//! @attention uncheckable requirement: @c T is not a dependent type, nor a template instanciation (checked by implementation)
	template<typename T>
	concept simple_udt = std::is_enum_v<T> or std::is_class_v<T> or std::is_union_v<T>;
}

namespace meta::internal {
	template<simple_udt T>
	consteval
	auto compute_typename() noexcept {
#if defined(_MSC_VER)
		constexpr std::string_view func{__FUNCSIG__};
#elif defined(__GNUC__) || defined(__clang__)
		constexpr std::string_view func{__PRETTY_FUNCTION__};
#else
	#error "Unsupported compiler"
#endif
		constexpr auto full{[](std::string_view decorated) {
#if defined(__clang__) || defined(__GNUC__)
			decorated.remove_suffix(1);
			decorated.remove_prefix(decorated.rfind('=') + 2);
#elif defined(_MSC_VER)
			decorated.remove_prefix(decorated.find('<') + 1);
			decorated.remove_suffix(decorated.size() - decorated.rfind('>'));
#endif
			//remove cv-qualifiers (not a constraint to simplify usage)
			if constexpr(std::is_const_v<T>) decorated.remove_prefix(6);
			if constexpr(std::is_volatile_v<T>) decorated.remove_prefix(9);
#if defined(_MSC_VER)
			//remove type-prefixes
			if constexpr(std::is_enum_v<T>) decorated.remove_prefix(5);
			else if constexpr(std::is_union_v<T>) decorated.remove_prefix(6);
			else if constexpr(std::is_class_v<T>) decorated.remove_prefix(decorated.front() == 'c' ? 6 : 7); //"class " or "struct "
#endif
			return decorated;
		}(func)};

		//can only support non-templates as otherwise there is too much divergence...
		static_assert(full.find('<') == full.npos); //TODO: [C++23] use not full.contains('<')

		constexpr auto name{[](std::string_view full) {
			if(const auto ind{full.rfind(':')}; ind != full.npos) full.remove_prefix(ind + 1);
			return full;
		}(full)};
		fixed_string<name.size()> n{name};

		constexpr auto count{[](std::string_view full) {
			std::size_t i{0};
			for(const auto & c : full)
				if(c == ':')
					++i;
			return i / 2;
		}(full)};
		constexpr auto scopes{[&]<auto... I>(std::index_sequence<I...>) {
			constexpr std::array<std::string_view, sizeof...(I)> scopes{
				[](std::size_t i, auto full) {
					for(; i; --i) full.remove_prefix(full.find(':') + 2);
					full.remove_suffix(full.size() - full.find(':'));
					return full;
				}(I, full) ...
			};
			(void)scopes; //silencing GCC warning in case I is empty pack...
			return std::make_tuple(fixed_string<scopes[I].size()>{scopes[I]}...);
		}(std::make_index_sequence<count>{})};

		return std::make_pair(scopes, n);
	}
}

namespace meta {
	template<simple_udt T>
	inline
	constexpr
	auto scopes{internal::compute_typename<T>().first};

	template<simple_udt T>
	inline
	constexpr
	fixed_string type_name{internal::compute_typename<T>().second};
}
