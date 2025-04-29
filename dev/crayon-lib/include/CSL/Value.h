#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace crayon
{
	namespace csl
	{
		struct StringConstant
		{
			const char* data_ptr{ nullptr };
			int length{ 0 };
			int id{ -1 };
		};

		struct StringConstant2
		{
			std::string_view data{};
			int id{ -1 };
		};

		enum class NumericType
		{
			INT, UINT,
			FLOAT, DOUBLE,
			BOOL
		};

		struct NumericConstant
		{
			std::array<unsigned char, 8> data;
			NumericType type{};
			int id{ -1 };
		};

		struct NumericConstant2
		{
			NumericType type{};
			int id{ -1 };
			union
			{
				uint64_t u64;
				int64_t i64;
				double f64;
			} as;
		};
	}
}