
//          Copyright Michael Florian Hava.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <ostream>
#include <string_view>

namespace meta {
	template<std::size_t N>
	struct fixed_string final {
		char buffer[N];

		consteval
		fixed_string(std::string_view sv) noexcept {
			sv.copy(buffer, N);
		}

		//TODO: additional features?

		constexpr
		operator std::string_view() const noexcept { return {buffer, N}; }

		friend
		constexpr
		auto operator==(const fixed_string & lhs, std::string_view rhs) noexcept -> bool {
			return std::string_view{lhs} == rhs;
		}

		friend
		auto operator<<(std::ostream & os, const fixed_string & self) -> std::ostream & {
			os << std::string_view{self};
			return os;
		}
	};
}

