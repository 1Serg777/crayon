#include "Utility.h"

static constexpr std::string_view cslExt{ ".csl" };

namespace crayon {

	bool FileExtCsl(std::string_view ext) {
		return ext == cslExt;
	}
	size_t CalcDigitCount(size_t number) {
		size_t num = number;
		size_t count = 0;
		while (num != 0) {
			num = number / 10;
			count++;
		}
		return count;
	}

}