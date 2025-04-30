#include "GLSL/Compiler.h"

#include "Utility.h"

#include <fstream>

namespace crayon
{
	namespace glsl
	{
		Compiler::Compiler()
		{
			lexer = std::make_unique<Lexer>();
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

			// 1. Lexing

			try
			{
				lexer->ScanSrcCode(srcCodeData.data(), srcCodeData.size());
			}
			catch (std::runtime_error& err)
			{
				std::cerr << err.what() << std::endl;
			}

			const std::vector<Token>& tokens = lexer->GetTokens();
			std::cout << "Tokens: ";
			PrintTokens(tokens);
			std::cout << "\n";
			
			// 2. Parsing

			try
			{
				parser->Parse(tokens.data(), tokens.size());
			}
			catch (std::runtime_error& err)
			{
				std::cerr << err.what() << std::endl;
			}

			std::shared_ptr<Expr> rootExpr = parser->GetRootExpression();

			// 3. Expression evaluation
			// [TODO]

			ExprEvalVisitor exprEvalVisitor{};
			rootExpr->Accept(&exprEvalVisitor);
			std::cout << "Result: " << exprEvalVisitor.GetResult() << "\n";

			// 4. Expression Postfix Notation printer

			ExprPostfixPrinterVisitor exprPostfixPrinter{};
			std::cout << "Postfix notation: ";
			rootExpr->Accept(&exprPostfixPrinter);

			std::cout << std::endl;
		}

		void Compiler::PrintTokens(const std::vector<Token>& tokens)
		{
			for (const Token& token : tokens)
			{
				PrintToken(std::cout, token);
				std::cout << " ";
			}
		}
	}
}