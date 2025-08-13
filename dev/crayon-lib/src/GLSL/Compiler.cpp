#include "GLSL/Compiler.h"
#include "GLSL/CodeGen/GlslWriter.h"
#include "GLSL/CodeGen/GlslExtWriter.h"
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

		constexpr std::string_view constKeyword  {"const"};
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
		constexpr std::string_view voidKeyword  {"void"  };

		constexpr std::string_view boolKeyword  {"bool"  };
		constexpr std::string_view intKeyword   {"int"   };
		constexpr std::string_view uintKeyword  {"uint"  };
		constexpr std::string_view floatKeyword {"float" };
		constexpr std::string_view doubleKeyword{"double"};

		constexpr std::string_view bvec2Keyword{"bvec2"};
		constexpr std::string_view ivec2Keyword{"ivec2"};
		constexpr std::string_view uvec2Keyword{"uvec2"};
		constexpr std::string_view vec2Keyword {"vec2" };
		constexpr std::string_view dvec2Keyword{"dvec2"};

		constexpr std::string_view bvec3Keyword{"bvec3"};
		constexpr std::string_view ivec3Keyword{"ivec3"};
		constexpr std::string_view uvec3Keyword{"uvec3"};
		constexpr std::string_view vec3Keyword {"vec3" };
		constexpr std::string_view dvec3Keyword{"dvec3"};

		constexpr std::string_view bvec4Keyword{"bvec4"};
		constexpr std::string_view ivec4Keyword{"ivec4"};
		constexpr std::string_view uvec4Keyword{"uvec4"};
		constexpr std::string_view vec4Keyword {"vec4" };
		constexpr std::string_view dvec4Keyword{"dvec4"};

		constexpr std::string_view mat2Keyword {"mat2" };
		constexpr std::string_view mat3Keyword {"mat3" };
		constexpr std::string_view mat4Keyword {"mat4" };
		constexpr std::string_view dmat2Keyword{"dmat2"};
		constexpr std::string_view dmat3Keyword{"dmat3"};
		constexpr std::string_view dmat4Keyword{"dmat4"};

		constexpr std::string_view mat2x2Keyword {"mat2x2" };
		constexpr std::string_view dmat2x2Keyword{"dmat2x2"};
		constexpr std::string_view mat2x3Keyword {"mat2x3" };
		constexpr std::string_view dmat2x3Keyword{"dmat2x3"};
		constexpr std::string_view mat2x4Keyword {"mat2x4" };
		constexpr std::string_view dmat2x4Keyword{"dmat2x4"};

		constexpr std::string_view mat3x2Keyword {"mat3x2" };
		constexpr std::string_view dmat3x2Keyword{"dmat3x2"};
		constexpr std::string_view mat3x3Keyword {"mat3x3" };
		constexpr std::string_view dmat3x3Keyword{"dmat3x3"};
		constexpr std::string_view mat3x4Keyword {"mat3x4" };
		constexpr std::string_view dmat3x4Keyword{"dmat3x4"};

		constexpr std::string_view mat4x2Keyword {"mat4x2" };
		constexpr std::string_view dmat4x2Keyword{"dmat4x2"};
		constexpr std::string_view mat4x3Keyword {"mat4x3" };
		constexpr std::string_view dmat4x3Keyword{"dmat4x3"};
		constexpr std::string_view mat4x4Keyword {"mat4x4" };
		constexpr std::string_view dmat4x4Keyword{"dmat4x4"};

		// ... add more types later ...
		// opaque types, more specifically

		constexpr std::string_view uimage2dmsarrayKeyword{"uimage2DMSArray"};

		// GLSL language extension keywords:

		constexpr std::string_view shaderProgramKeyword    {"ShaderProgram"};
		constexpr std::string_view beginKeyword            {"BEGIN"        };
		constexpr std::string_view endKeyword              {"END"          };
		// Graphics pipeline blocks:
		constexpr std::string_view fixedStagesConfigKeyword {"FixedStagesConfig" };
		constexpr std::string_view materialPropertiesKeyword{"MaterialProperties"};
		constexpr std::string_view vertexInputLayoutKeyword {"VertexInputLayout" };
		constexpr std::string_view colorAttachmentsKeyword  {"ColorAttachments" };
		// Material property type keywords:
		constexpr std::string_view materialPropertyTypeIntKeyword  {"Integer"  };
		constexpr std::string_view materialPropertyTypeFloatKeyword{"Float"    };
		constexpr std::string_view materialPropertyTypeVec2Keyword {"Vector2"  };
		constexpr std::string_view materialPropertyTypeVec3Keyword {"Vector3"  };
		constexpr std::string_view materialPropertyTypeVec4Keyword {"Vector4"  };
		constexpr std::string_view materialPropertyTypeColorKeyword{"Color"    };
		constexpr std::string_view materialPropertyTypeTex2DKeyword{"Texture2D"};
		// Graphics pipeline shader stages:
		constexpr std::string_view vertexShaderKeyword                {"VertexShader"                };
		constexpr std::string_view tessellationControlShaderKeyword   {"TessellationControlShader"   };
		constexpr std::string_view tessellationEvaluationShaderKeyword{"TessellationEvaluationShader"};
		constexpr std::string_view geometryShaderKeyword              {"GeometryShader"              };
		constexpr std::string_view fragmentShaderKeyword              {"FragmentShader"              };


		Compiler::Compiler() {
			lexer = std::make_unique<Lexer>();
			parser = std::make_unique<Parser>();
			errorReporter = std::make_unique<ErrorReporter>();
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

			errorReporter->SetSrcCodeLink(srcCodeData.data(), srcCodeData.size());

			GpuApiType gpuApiType{GpuApiType::VULKAN};

			// 1. Lexing

			LexerConfig lexConfig{};
			lexConfig.keywords = &keywords;
			lexConfig.gpuApiType = gpuApiType;
			try {
				lexer->Scan(srcCodeData.data(), srcCodeData.size(), lexConfig);
			} catch (std::runtime_error& err) {
				std::cerr << err.what() << std::endl;
				return;
			}
			std::cout << "Tokens:\n";
			PrintTokens(lexer->GetTokenData(), lexer->GetTokenSize());
			
			// 2. Parsing

			ParserConfig parserConfig{};
			parserConfig.errorReporter = errorReporter.get();
			parserConfig.gpuApiType = gpuApiType;
			try {
				parser->Parse(lexer->GetTokenData(), lexer->GetTokenSize(), parserConfig);
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
			defaultConfig.openingBraceOnSameLine = true;

			// std::shared_ptr<GlslWriter> glslWriter = std::make_shared<GlslWriter>(defaultConfig);

			// 1. Translation unit alone.
			// std::shared_ptr<TransUnit> transUnit = parser->GetTranslationUnit();
			// glslWriter->VisitTransUnit(transUnit.get());

			// 2. Shader program block.
			// std::shared_ptr<ShaderProgramBlock> shaderProgramBlock = parser->GetShaderProgramBlock();
			// shaderProgramBlock->Accept(glslWriter.get());

			// std::filesystem::path genSrcCodePath = srcCodePath.parent_path() / "gen_src.csl";
			// std::ofstream genSrcCodeFile{genSrcCodePath, std::ifstream::out | std::ifstream::binary};
			// genSrcCodeFile << glslWriter->GetSrcCodeStr();

			// 3. Generating vertex and fragment shaders source code.
			//    We parsed extended GLSL source code, but we're going to produce only core GLSL source code.

			std::shared_ptr<GlslExtWriter> glslExtWriter = std::make_shared<GlslExtWriter>(defaultConfig);
			std::shared_ptr<ShaderProgramBlock> shaderProgramBlock = parser->GetShaderProgramBlock();
			std::shared_ptr<ShaderProgram> shaderProgram = glslExtWriter->CompileToGlsl(shaderProgramBlock.get());
			
			if (shaderProgram->HasVertexShaderSrc()) {
				const std::string vsSrc = shaderProgram->GetVertexShaderSrc();
				// std::filesystem::path vsSrcPath = srcCodePath.parent_path() / "gen_src_vs.csl";
				std::filesystem::path vsSrcPath = srcCodePath.parent_path() / "generated.vs";
				std::ofstream vsSrcCodeFile{vsSrcPath, std::ifstream::out | std::ifstream::binary};
				vsSrcCodeFile << vsSrc;
			}
			if (shaderProgram->HasFragmentShaderSrc()) {
				const std::string fsSrc = shaderProgram->GetFragmentShaderSrc();
				std::filesystem::path fsSrcPath = srcCodePath.parent_path() / "generated.fs";
				std::ofstream fsSrcCodeFile{fsSrcPath, std::ifstream::out | std::ifstream::binary};
				fsSrcCodeFile << fsSrc;
			}
		}

		void Compiler::InitializeKeywordMap() {
			// Type qualifier keywords

			// Layout qualifier keywords

			keywords.insert({layoutKeyword, TokenType::LAYOUT});

			// Storage qualifier keywords

			keywords.insert({constKeyword,   TokenType::CONST  });
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
			keywords.insert({intKeyword,    TokenType::INT   });
			keywords.insert({uintKeyword,   TokenType::UINT  });
			keywords.insert({floatKeyword,  TokenType::FLOAT });
			keywords.insert({doubleKeyword, TokenType::DOUBLE});

			keywords.insert({bvec2Keyword, TokenType::BVEC2});
			keywords.insert({ivec2Keyword, TokenType::IVEC2});
			keywords.insert({uvec2Keyword, TokenType::UVEC2});
			keywords.insert({vec2Keyword,  TokenType::VEC2 });
			keywords.insert({dvec2Keyword, TokenType::DVEC2});
			
			keywords.insert({bvec3Keyword, TokenType::BVEC3});
			keywords.insert({ivec3Keyword, TokenType::IVEC3});
			keywords.insert({uvec3Keyword, TokenType::UVEC3});
			keywords.insert({vec3Keyword,  TokenType::VEC3 });
			keywords.insert({dvec3Keyword, TokenType::DVEC3});
			
			keywords.insert({bvec4Keyword, TokenType::BVEC4});
			keywords.insert({ivec4Keyword, TokenType::IVEC4});
			keywords.insert({uvec4Keyword, TokenType::UVEC4});
			keywords.insert({vec4Keyword,  TokenType::VEC4 });
			keywords.insert({dvec4Keyword, TokenType::DVEC4});
			
			keywords.insert({mat2Keyword,  TokenType::MAT2 });
			keywords.insert({mat3Keyword,  TokenType::MAT3 });
			keywords.insert({mat4Keyword,  TokenType::MAT4 });
			keywords.insert({dmat2Keyword, TokenType::DMAT2});
			keywords.insert({dmat3Keyword, TokenType::DMAT3});
			keywords.insert({dmat4Keyword, TokenType::DMAT4});
			
			keywords.insert({mat2x2Keyword,  TokenType::MAT2X2 });
			keywords.insert({dmat2x2Keyword, TokenType::DMAT2X2});
			keywords.insert({mat2x3Keyword,  TokenType::MAT2X3 });
			keywords.insert({dmat2x3Keyword, TokenType::DMAT2X3});
			keywords.insert({mat2x4Keyword,  TokenType::MAT2X4 });
			keywords.insert({dmat2x4Keyword, TokenType::DMAT2X4});
			
			keywords.insert({mat3x2Keyword,  TokenType::MAT3X2 });
			keywords.insert({dmat3x2Keyword, TokenType::DMAT3X2});
			keywords.insert({mat3x3Keyword,  TokenType::MAT3X3 });
			keywords.insert({dmat3x3Keyword, TokenType::DMAT3X3});
			keywords.insert({mat3x4Keyword,  TokenType::MAT3X4 });
			keywords.insert({dmat3x4Keyword, TokenType::DMAT3X4});
			
			keywords.insert({mat4x2Keyword,  TokenType::MAT4X2 });
			keywords.insert({dmat4x2Keyword, TokenType::DMAT4X2});
			keywords.insert({mat4x3Keyword,  TokenType::MAT4X3 });
			keywords.insert({dmat4x3Keyword, TokenType::DMAT4X3});
			keywords.insert({mat4x4Keyword,  TokenType::MAT4X4 });
			keywords.insert({dmat4x4Keyword, TokenType::DMAT4X4});

			// ... add more types later ...
			// opaque types, more specifically

			keywords.insert({uimage2dmsarrayKeyword, TokenType::UIMAGE2DMSARRAY});

			// GLSL language extension keywords:

			keywords.insert({shaderProgramKeyword, TokenType::SHADER_PROGRAM_KW});
			keywords.insert({beginKeyword,         TokenType::BEGIN            });
			keywords.insert({endKeyword,           TokenType::END              });
			// Graphics pipeline blocks:
			keywords.insert({fixedStagesConfigKeyword,  TokenType::FIXED_STAGES_CONFIG_KW});
			keywords.insert({materialPropertiesKeyword, TokenType::MATERIAL_PROPERTIES_KW});
			keywords.insert({vertexInputLayoutKeyword,  TokenType::VERTEX_INPUT_LAYOUT_KW});
			keywords.insert({colorAttachmentsKeyword,   TokenType::COLOR_ATTACHMENTS_KW  });
			// Material property types:
			keywords.insert({materialPropertyTypeIntKeyword,   TokenType::MAT_PROP_TYPE_INT  });
			keywords.insert({materialPropertyTypeFloatKeyword, TokenType::MAT_PROP_TYPE_FLOAT});
			keywords.insert({materialPropertyTypeVec2Keyword,  TokenType::MAT_PROP_TYPE_VEC2 });
			keywords.insert({materialPropertyTypeVec3Keyword,  TokenType::MAT_PROP_TYPE_VEC3 });
			keywords.insert({materialPropertyTypeVec4Keyword,  TokenType::MAT_PROP_TYPE_VEC4 });
			keywords.insert({materialPropertyTypeColorKeyword, TokenType::MAT_PROP_TYPE_COLOR});
			keywords.insert({materialPropertyTypeTex2DKeyword, TokenType::MAT_PROP_TYPE_TEX2D});
			// Graphics pipeline shader stages:
			keywords.insert({vertexShaderKeyword,                 TokenType::VS_KW });
			keywords.insert({tessellationControlShaderKeyword,    TokenType::TCS_KW});
			keywords.insert({tessellationEvaluationShaderKeyword, TokenType::TES_KW});
			keywords.insert({geometryShaderKeyword,               TokenType::GS_KW });
			keywords.insert({fragmentShaderKeyword,               TokenType::FS_KW });
		}

		void Compiler::PrintTokens(const Token* tokenData, size_t tokenSize) {
			for (size_t i = 0; i < tokenSize; i++) {
				PrintToken(std::cout, tokenData[i]);
				std::cout << "\n";
			}
		}
	}
}