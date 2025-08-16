#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace crayon {
	namespace spirv {

		enum class SpvOpCode : uint16_t {
			OpNop               = 0,
			// Debug instructions.
			OpSourceContinued   = 2,
			OpSource            = 3,
			OpName              = 5,
			OpMemberName        = 6,
			OpString            = 7,
			OpLine              = 8,
			// Annotation instructions.
			OpDecorate          = 71,
			OpMemberDecorate    = 72,
			// Extension instructions.
			OpExtension         = 10,
			// Mode-setting instructions.
			OpMemoryModel       = 14,
			OpEntryPoint        = 15,
			OpExecutionMode     = 16,
			OpCapability        = 17,
			// Type declaration instructions.
			OpTypeVoid          = 19,
			OpTypeBool          = 20,
			OpTypeInt           = 21,
			OpTypeFloat         = 22,
			OpTypeVector        = 23,
			OpTypeMatrix        = 24,
			OpTypeImage         = 25,
			OpTypeSampler       = 26,
			OpTypeArray         = 28,
			OpTypeStruct        = 30,
			OpTypePointer       = 32,
			OpTypeFunction      = 33,
			// Constant-creation instructions.
			OpConstantTrue      = 41,
			OpConstantFalse     = 42,
			OpConstant          = 43,
			OpConstantComposite = 44,
			// Memory instructions.
			OpVariable          = 59,
			OpLoad              = 61,
			OpStore             = 62,
			OpAccessChain       = 65,
			// Function instrructions.
			OpFunction          = 54,
			OpFunctionParameter = 55,
			OpFunctionEnd       = 56,
			OpFunctionCall      = 57,
			// Image instructions.
			// TODO
			// Conversion instructions.
			OpConvertFToU       = 109,
			OpConvertFToS       = 110,
			OpConvertSToF       = 111,
			OpconvertUToF       = 112,
			OpUConvert          = 113,
			OpSConvert          = 114,
			OpFConvert          = 115,
			// Arithmetic instructions.
			OpSNegate           = 126,
			OpFNegate           = 127,
			OpIAdd              = 128,
			OpFAdd              = 129,
			OpISub              = 130,
			OpFSub              = 131,
			OpIMul              = 132,
			OpFMul              = 133,
			OpUDiv              = 134,
			OpSDiv              = 135,
			OpFDiv              = 136,
			OpVectorTimesScalar = 142,
			OpMatrixTimesScalar = 143,
			OpVectorTimesMatrix = 144,
			OpMatrixTimesVector = 145,
			OpMatrixTimesMatrix = 146,
			OpOuterProduct      = 147,
			OpDot               = 148,
			// Relational and logical instructions.
			OpLessOrGreater     = 161,
			OpLogicalEqual      = 164,
			OpLogicalNotEqual   = 165,
			OpLogicalOr         = 166,
			OpLogicalAnd        = 167,
			OpLogicalNot        = 168,
			OpIEqual            = 170,
			OpINotEqual         = 171,
			OpUGreaterThan      = 172,
			OpSGreaterThan      = 173,
			OpUGreaterThanEqual = 174,
			OpSGreaterThanEqual = 175,
			OpULessThan         = 176,
			OpSLessThan         = 177,
			OpULessThanEqual    = 178,
			OpSLessThanEqual    = 179,
			// Control-flow instructions.
			OpLabel             = 248,
			OpReturn            = 253,
		};

		enum SpvScopeContext {
			EXTERNAL,
			FUNCTION,
			BLOCK,
		};

		std::string_view SpvOpCodeToString(SpvOpCode opCode);

	}
}