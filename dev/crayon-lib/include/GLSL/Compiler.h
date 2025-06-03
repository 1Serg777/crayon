#pragma once

#include "GLSL/Analyzer/Lexer.h"
#include "GLSL/Analyzer/Parser.h"

#include <filesystem>
#include <memory>
#include <string_view>
#include <unordered_map>

namespace crayon {
	namespace glsl {

		class Compiler {
		public:
			Compiler();

			void Compile(const std::filesystem::path& srcCodePath);

		private:
			void InitializeKeywordMap();

			void PrintTokens(const Token* tokenData, size_t tokenSize);

			std::unique_ptr<Lexer> lexer;
			std::unique_ptr<Parser> parser;

			std::unordered_map<std::string_view, TokenType> keywords;
		};
	}
}