#pragma once
#include <cstddef>
#include <cstdint>
#include <cwctype>
#include <algorithm>
#include <array>

// Code by David Krauss (Potatoswatter)
// http://coliru.stacked-crooked.com/a/8d35ee14f25cc47d

namespace Pi3CfastRead {

	std::uint64_t mystrtol(char const*& pen, std::uint64_t val = 0);
	double mystrtof(char const* pen);

}