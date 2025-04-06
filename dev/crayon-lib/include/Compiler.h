#pragma once

#include "Scanner.h"
#include "Parser.h"

#include <filesystem>
#include <memory>

namespace csl
{
	class Compiler
	{
	public:

		Compiler();
		~Compiler() = default;

		void Compile(const std::filesystem::path& srcCodePath);

	private:

		std::unique_ptr<Scanner> scanner;
		std::unique_ptr<ScannerErrorReporter> scannerErrorReporter;

		std::unique_ptr<Parser> parser;
	};
}