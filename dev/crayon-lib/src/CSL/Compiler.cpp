#include "CSL/Compiler.h"

#include "Utility.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

namespace crayon
{
	namespace csl
	{
		Compiler::Compiler()
		{
			scanner = std::make_unique<Scanner>();
			scannerErrorReporter = std::make_unique<ConsoleScannerErrorReporter>();

			parser = std::make_unique<Parser>();
		}

		void Compiler::Compile(const std::filesystem::path& srcCodePath)
		{
			std::string srcCodeFileExt = srcCodePath.extension().generic_string();

			if (!FileExtCsl(srcCodeFileExt))
			{
				std::string errMsg{ "File extension must be \".csl\"" };
				throw std::runtime_error{ errMsg };
			}

			std::ifstream srcCodeFile(srcCodePath, std::ifstream::in | std::ifstream::ate | std::ifstream::binary);

			if (!srcCodeFile.is_open())
			{
				std::string errMsg{ "Couldn't open the .csl source code file: " + srcCodePath.string() };
				throw std::runtime_error{ errMsg };
			}

			size_t fileSize = srcCodeFile.tellg();
			std::vector<char> srcCodeData(fileSize);

			srcCodeFile.seekg(0);
			srcCodeFile.read(srcCodeData.data(), fileSize);

			srcCodeFile.close();

			ScannerParams scannerParams{};
			scannerParams.errorReporter = scannerErrorReporter.get();

			try
			{
				scanner->ScanSrcCode(scannerParams, srcCodeData.data(), srcCodeData.size());
			}
			catch (std::runtime_error& err)
			{
				std::cerr << err.what() << std::endl;
			}

			const std::vector<Token>& tokens = scanner->GetTokens();
			for (const Token& token : tokens)
			{
				PrintToken(std::cout, token);
				std::cout << " ";
			}
		}
	}
}