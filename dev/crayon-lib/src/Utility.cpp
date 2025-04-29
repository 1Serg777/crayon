#include "Utility.h"

static constexpr std::string_view cslExt{ ".csl" };

namespace crayon
{
	bool FileExtCsl(std::string_view ext)
	{
		return ext == cslExt;
	}
}