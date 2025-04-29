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
			~Compiler() = default;

			void Compile(const std::filesystem::path& srcCodePath);

		private:

			std::unique_ptr<Lexer> scanner;
			std::unique_ptr<Parser> parser;
		};
	}
}