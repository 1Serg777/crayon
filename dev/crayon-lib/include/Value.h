#pragma once

#include <array>

namespace csl
{
	enum class BaseValueType
	{
		BOOL,
		INT,
		UINT,
		FLOAT,
		DOUBLE,
	};

	struct BaseValueConstant
	{
		std::array<unsigned char, 8> storage;
		int id{ -1 };
		BaseValueType type{};
	};
}