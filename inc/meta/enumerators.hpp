
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef METADATA_MAX_ENUMERATOR_VALUE
	#error "Missing METADATA_MAX_ENUMERATOR_VALUE"
#endif

#include <tuple>
#include <type_traits>
#include "fixed_string.hpp"

namespace meta::internal {
	//! @attention does not reliably work for enums defined inside a template
	template<auto E>
	consteval
	auto compute_enumerator_name() noexcept {
#if defined(_MSC_VER)
		constexpr std::string_view func{__FUNCSIG__};
#elif defined(__GNUC__) || defined(__clang__)
		constexpr std::string_view func{__PRETTY_FUNCTION__};
#else
	#error "Unsupported compiler"
#endif
		constexpr auto name{[](std::string_view decorated) {
#if defined(__clang__) || defined(__GNUC__)
			decorated.remove_suffix(1);
			decorated.remove_prefix(decorated.rfind('=') + 2);
			if(decorated[0] == '(') return std::string_view{}; //not a valid enumerator
			decorated.remove_prefix(decorated.rfind(':') + 1);
#elif defined(_MSC_VER)
			decorated.remove_suffix(decorated.size() - decorated.rfind('>'));
			if(decorated.rfind(')') != decorated.npos) return std::string_view{}; //not a valid enumerator
			decorated.remove_prefix(decorated.rfind(':') + 1);
#endif
			return decorated;
		}(func)};
		if constexpr(!name.empty()) return fixed_string<name.size()>{name};
	}


	inline
	constexpr
	struct {
		template<typename Underlying>
		consteval
		auto operator()(std::integer_sequence<Underlying> ints) const { return ints; }

		template<typename Underlying, Underlying... L, Underlying... R>
		consteval
		auto operator()(std::integer_sequence<Underlying, L...>, std::integer_sequence<Underlying, R...>) const { return std::integer_sequence<Underlying, L..., R...>{}; }

		template<typename Underlying, Underlying... L, Underlying... R, typename... Rest>
		consteval
		auto operator()(std::integer_sequence<Underlying, L...>, std::integer_sequence<Underlying, R...>, Rest... rest) const { return (*this)(std::integer_sequence<Underlying, L..., R...>{}, rest...); }
	} concat; //can unfortunately not be defined more local


	template<typename E>
	requires std::is_enum_v<E>
	consteval
	auto make_enumerator_sequence() noexcept {
		//generate [0, METADATA_MAX_ENUMERATOR_VALUE] for unsigned
		//generate [-METADATA_MAX_ENUMERATOR_VALUE, METADATA_MAX_ENUMERATOR_VALUE] for signed
		using Underlying = std::underlying_type_t<E>;
		constexpr std::make_integer_sequence<Underlying, METADATA_MAX_ENUMERATOR_VALUE + 1> positive;
		constexpr auto negative{[&] {
			if constexpr(std::is_unsigned_v<Underlying>) return std::integer_sequence<Underlying>{};
			else return []<auto... I>(std::integer_sequence<Underlying, 0, I...>) {
				return []<auto... J>(std::index_sequence<J...>) {
					return std::integer_sequence<Underlying, std::get<sizeof...(J) - J - 1>(std::tuple<decltype(I)...>{(-I)...})...>{};
				}(std::make_index_sequence<sizeof...(I)>{});
			}(positive); //filter 0, negate all other integers and reverse order
		}()};

		constexpr auto all{concat(negative, positive)}; //enumerators outside of negative + positive are ignored

		//filter for valid enumerators
		return []<Underlying... Is>(std::integer_sequence<Underlying, Is...>) {
			if constexpr(sizeof...(Is) == 0) return std::integer_sequence<Underlying>{};
			else {
				constexpr auto filter{[]<auto I>(std::integer_sequence<Underlying, I>) {
					if constexpr(std::same_as<void, decltype(compute_enumerator_name<static_cast<E>(I)>())>) return std::integer_sequence<Underlying>{};
					else return std::integer_sequence<Underlying, I>{};
				}};

				return concat(filter(std::integer_sequence<Underlying, Is>{})...);
			}
		}(all);
	}
}

namespace meta {
	template<typename E, E I>
	requires std::is_enum_v<E>
	struct enumerator final {
		inline
		static
		constexpr
		fixed_string name{internal::compute_enumerator_name<I>()};

		inline
		static
		constexpr
		E value{I};
	};


	template<typename E>
	requires std::is_enum_v<E>
	using enumerators = decltype([]<auto... I>(std::integer_sequence<std::underlying_type_t<E>, I...>) { return std::make_tuple(enumerator<E, static_cast<E>(I)>{}...); }(internal::make_enumerator_sequence<E>()));
}
