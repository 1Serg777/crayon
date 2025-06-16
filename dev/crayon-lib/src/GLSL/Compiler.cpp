#include "GLSL/Compiler.h"
#include "GLSL/CodeGen/GlslWriter.h"
#include "Utility.h"

#include <cassert>
#include <fstream>

namespace crayon
{
	namespace glsl
	{
		// Type qualifier keywords

		// Layout qualifier keywords

		constexpr std::string_view layoutKeyword{"layout"};

		// Storage qualifier keywords

		constexpr std::string_view inKeyword     {"in"};
		constexpr std::string_view outKeyword    {"out"};
		constexpr std::string_view uniformKeyword{"uniform"};
		constexpr std::string_view bufferKeyword {"buffer"};

		// Interpolation qualifier keywords

		constexpr std::string_view smoothKeyword       {"smooth"};
		constexpr std::string_view flatKeyword         {"flat"};
		constexpr std::string_view noperspectiveKeyword{"noperspective"};

		// Type keywords

		constexpr std::string_view structKeyword{"struct"};
		constexpr std::string_view voidKeyword  {"void"};

		constexpr std::string_view boolKeyword  {"int"};
		constexpr std::string_view floatKeyword {"float"};
		constexpr std::string_view intKeyword   {"int"};
		constexpr std::string_view uintKeyword  {"int"};
		constexpr std::string_view doubleKeyword{"int"};

		constexpr std::string_view bvec2Keyword{"bvec2"};
		constexpr std::string_view vec2Keyword {"vec2"};
		constexpr std::string_view vec3Keyword {"vec3"};
		constexpr std::string_view vec4Keyword {"vec4"};
		constexpr std::string_view dvec2Keyword{"dvec2"};
		constexpr std::string_view dvec3Keyword{"dvec3"};
		constexpr std::string_view dvec4Keyword{"dvec4"};

		constexpr std::string_view mat2Keyword   {"mat2"};
		constexpr std::string_view dmat4x4Keyword{"dmat4x4"};

		// ... add more types later ... 

		constexpr std::string_view uimage2dmsarrayKeyword{"uimage2DMSArray"};

		Compiler::Compiler() {
			lexer = std::make_unique<Lexer>();
			parser = std::make_unique<Parser>();
			InitializeKeywordMap();
		}

		void Compiler::Compile(const std::filesystem::path& srcCodePath) {
			std::string srcCodeFileExt = srcCodePath.extension().generic_string();
			if (!FileExtCsl(srcCodeFileExt)) {
				std::string errMsg{ "File extension must be \".csl\"" };
				throw std::runtime_error{ errMsg };
			}

			std::ifstream srcCodeFile(
				srcCodePath, std::ifstream::in | std::ifstream::ate | std::ifstream::binary);
			if (!srcCodeFile.is_open()) {
				std::string errMsg{ "Couldn't open the .csl source code file: " + srcCodePath.string() };
				throw std::runtime_error{ errMsg };
			}

			size_t fileSize = srcCodeFile.tellg();
			std::vector<char> srcCodeData(fileSize);
			srcCodeFile.seekg(0);
			srcCodeFile.read(srcCodeData.data(), fileSize);
			srcCodeFile.close();

			// 1. Lexing

			LexerConfig lexConfig{};
			lexConfig.keywords = &keywords;
			try {
				lexer->Scan(srcCodeData.data(), srcCodeData.size(), lexConfig);
			} catch (std::runtime_error& err) {
				std::cerr << err.what() << std::endl;
				return;
			}
			std::cout << "Tokens:\n";
			PrintTokens(lexer->GetTokenData(), lexer->GetTokenSize());
			
			// 2. Parsing

			try {
				parser->Parse(lexer->GetTokenData(), lexer->GetTokenSize());
			} catch (std::runtime_error& err) {
				std::cerr << err.what() << std::endl;
				return;
			}

			// Expressions test
			/*
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

			ExprParenPrinterVisitor exprParenPrinter{};
			std::cout << "Parenthesized equivalent: ";
			rootExpr->Accept(&exprParenPrinter);
			std::cout << std::endl;
			*/

			// Statements test
			// const std::vector<std::shared_ptr<Stmt>>& stmts = parser->GetStatements();

			// Source Code Generation

			if (parser->HadSyntaxError()) {
				return;
			}

			GlslWriterConfig defaultConfig{};
			std::shared_ptr<GlslWriter> glslWriter = std::make_shared<GlslWriter>(defaultConfig);

			std::shared_ptr<TransUnit> transUnit = parser->GetTranslationUnit();
			glslWriter->VisitTransUnit(transUnit.get());

			std::filesystem::path genSrcCodePath = srcCodePath.parent_path() / "gen_src.csl";
			std::ofstream genSrcCodeFile{genSrcCodePath, std::ifstream::out | std::ifstream::binary};
			genSrcCodeFile << glslWriter->GetSrcCodeStr();
		}

		void Compiler::InitializeKeywordMap() {
			// Type qualifier keywords

			// Layout qualifier keywords

			keywords.insert({layoutKeyword, TokenType::LAYOUT});

			// Storage qualifier keywords

			keywords.insert({inKeyword,      TokenType::IN     });
			keywords.insert({outKeyword,     TokenType::OUT    });
			keywords.insert({uniformKeyword, TokenType::UNIFORM});
			keywords.insert({bufferKeyword,  TokenType::BUFFER });

			// Interpolation qualifier keywords

			keywords.insert({smoothKeyword,        TokenType::SMOOTH       });
			keywords.insert({flatKeyword,          TokenType::FLAT         });
			keywords.insert({noperspectiveKeyword, TokenType::NOPERSPECTIVE});

			// Type keywords

			keywords.insert({structKeyword, TokenType::STRUCT});
			keywords.insert({voidKeyword,   TokenType::VOID  });

			keywords.insert({boolKeyword,   TokenType::BOOL  });
			keywords.insert({floatKeyword,  TokenType::FLOAT });
			keywords.insert({intKeyword,    TokenType::INT   });
			keywords.insert({uintKeyword,   TokenType::UINT  });
			keywords.insert({doubleKeyword, TokenType::DOUBLE});

			keywords.insert({bvec2Keyword, TokenType::BVEC2});
			keywords.insert({vec2Keyword,  TokenType::VEC2 });
			keywords.insert({vec3Keyword,  TokenType::VEC3 });
			keywords.insert({vec4Keyword,  TokenType::VEC4 });
			keywords.insert({dvec2Keyword, TokenType::DVEC2});
			keywords.insert({dvec3Keyword, TokenType::DVEC3});
			keywords.insert({dvec4Keyword, TokenType::DVEC4});

			keywords.insert({mat2Keyword,    TokenType::MAT2   });
			keywords.insert({dmat4x4Keyword, TokenType::DMAT4X4});

			// ... add more types later ...

			keywords.insert({uimage2dmsarrayKeyword, TokenType::UIMAGE2DMSARRAY});
		}

		void Compiler::PrintTokens(const Token* tokenData, size_t tokenSize) {
			for (size_t i = 0; i < tokenSize; i++) {
				PrintToken(std::cout, tokenData[i]);
				std::cout << "\n";
			}
		}
	}
}