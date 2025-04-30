#pragma once

#include "GLSL/Lexer.h"
#include "GLSL/Parser.h"

#include <filesystem>
#include <memory>

namespace crayon
{
	namespace glsl
	{
		class Compiler
		{
		public:

			Compiler();

			void Compile(const std::filesystem::path& srcCodePath);

		private:

			void PrintTokens(const std::vector<Token>& tokens);

			std::unique_ptr<Lexer> lexer;
			std::unique_ptr<Parser> parser;
		};
	}
}