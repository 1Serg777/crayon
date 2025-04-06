#include "Utility.h"

static constexpr std::string_view cslExt{ ".csl" };

namespace csl
{
	bool FileExtCsl(std::string_view ext)
	{
		return ext == cslExt;
	}
}