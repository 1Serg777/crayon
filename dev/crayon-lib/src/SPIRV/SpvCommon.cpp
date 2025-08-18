#include "SPIRV/SpvCommon.h"

#include <cassert>
#include <unordered_map>

namespace crayon {
	namespace spirv {

		static const std::unordered_map<SpvOpCode, std::string_view> spvOpCodeMap = {
			{SpvOpCode::OpNop,               "OpNop"              },
			// Debug instructions.
			{SpvOpCode::OpSourceContinued,   "OpSourceContinued"  },
			{SpvOpCode::OpSource,            "OpSource"           },
			{SpvOpCode::OpName,              "OpName"             },
			{SpvOpCode::OpMemberName,        "OpMemberName"       },
			{SpvOpCode::OpString,            "OpString"           },
			{SpvOpCode::OpLine,              "OpLine"             },
			// Annotation instructions.
			{SpvOpCode::OpDecorate,          "OpDecorate"         },
			{SpvOpCode::OpMemberDecorate,    "OpMemberDecorate"   },
			// Extension instructions.
			{SpvOpCode::OpExtension,         "OpExtension"        },
			{SpvOpCode::OpExtInstImport,     "OpExtInstImport"    },
			// Mode-setting instructions.
			{SpvOpCode::OpMemoryModel,       "OpMemoryModel"      },
			{SpvOpCode::OpEntryPoint,        "OpEntryPoint"       },
			{SpvOpCode::OpExecutionMode,     "OpExecutionMode"    },
			{SpvOpCode::OpCapability,        "OpCapability"       },
			// Type declaration instructions.
			{SpvOpCode::OpTypeVoid,          "OpTypeVoid"         },
			{SpvOpCode::OpTypeBool,          "OpTypeBool"         },
			{SpvOpCode::OpTypeInt,           "OpTypeInt"          },
			{SpvOpCode::OpTypeFloat,         "OpTypeFloat"        },
			{SpvOpCode::OpTypeVector,        "OpTypeVector"       },
			{SpvOpCode::OpTypeMatrix,        "OpTypeMatrix"       },
			{SpvOpCode::OpTypeImage,         "OpTypeImage"        },
			{SpvOpCode::OpTypeSampler,       "OpTypeSampler"      },
			{SpvOpCode::OpTypeArray,         "OpTypeArray"        },
			{SpvOpCode::OpTypeStruct,        "OpTypeStruct"       },
			{SpvOpCode::OpTypePointer,       "OpTypePointer"      },
			{SpvOpCode::OpTypeFunction,      "OpTypeFunction"     },
			// Constant-creation instructions.
			{SpvOpCode::OpConstantTrue,      "OpConstantTrue"     },
			{SpvOpCode::OpConstantFalse,     "OpConstantFalse"    },
			{SpvOpCode::OpConstant,          "OpConstant"         },
			{SpvOpCode::OpConstantComposite, "OpConstantComposite"},
			// Memory instructions.
			{SpvOpCode::OpVariable,          "OpVariable"         },
			{SpvOpCode::OpLoad,              "OpLoad"             },
			{SpvOpCode::OpStore,             "OpStore"            },
			{SpvOpCode::OpAccessChain,       "OpAccessChain"      },
			// Function instrructions.
			{SpvOpCode::OpFunction,          "OpFunction"         },
			{SpvOpCode::OpFunctionParameter, "OpFunctionParameter"},
			{SpvOpCode::OpFunctionEnd,       "OpFunctionEnd"      },
			{SpvOpCode::OpFunctionCall,      "OpFunctionCall"     },
			// Image instructions.
			// TODO
			// Conversion instructions.
			{SpvOpCode::OpConvertFToU,       "OpConvertFToU"      },
			{SpvOpCode::OpConvertFToS,       "OpConvertFToS"      },
			{SpvOpCode::OpConvertSToF,       "OpConvertSToF"      },
			{SpvOpCode::OpconvertUToF,       "OpconvertUToF"      },
			{SpvOpCode::OpUConvert,          "OpUConvert"         },
			{SpvOpCode::OpSConvert,          "OpSConvert"         },
			{SpvOpCode::OpFConvert,          "OpFConvert"         },
			// Arithmetic instructions}.
			{SpvOpCode::OpSNegate,           "OpSNegate"          },
			{SpvOpCode::OpFNegate,           "OpFNegate"          },
			{SpvOpCode::OpIAdd,              "OpIAdd"             },
			{SpvOpCode::OpFAdd,              "OpFAdd"             },
			{SpvOpCode::OpISub,              "OpISub"             },
			{SpvOpCode::OpFSub,              "OpFSub"             },
			{SpvOpCode::OpIMul,              "OpIMul"             },
			{SpvOpCode::OpFMul,              "OpFMul"             },
			{SpvOpCode::OpUDiv,              "OpUDiv"             },
			{SpvOpCode::OpSDiv,              "OpSDiv"             },
			{SpvOpCode::OpFDiv,              "OpFDiv"             },
			{SpvOpCode::OpVectorTimesScalar, "OpVectorTimesScalar"},
			{SpvOpCode::OpMatrixTimesScalar, "OpMatrixTimesScalar"},
			{SpvOpCode::OpVectorTimesMatrix, "OpVectorTimesMatrix"},
			{SpvOpCode::OpMatrixTimesVector, "OpMatrixTimesVector"},
			{SpvOpCode::OpMatrixTimesMatrix, "OpMatrixTimesMatrix"},
			{SpvOpCode::OpOuterProduct,      "OpOuterProduct"     },
			{SpvOpCode::OpDot,               "OpDot"              },
			// Relational and logical instructions.
			{SpvOpCode::OpLessOrGreater,     "OpLessOrGreater"    },
			{SpvOpCode::OpLogicalEqual,      "OpLogicalEqual"     },
			{SpvOpCode::OpLogicalNotEqual,   "OpLogicalNotEqual"  },
			{SpvOpCode::OpLogicalOr,         "OpLogicalOr"        },
			{SpvOpCode::OpLogicalAnd,        "OpLogicalAnd"       },
			{SpvOpCode::OpLogicalNot,        "OpLogicalNot"       },
			{SpvOpCode::OpIEqual,            "OpIEqual"           },
			{SpvOpCode::OpINotEqual,         "OpINotEqual"        },
			{SpvOpCode::OpUGreaterThan,      "OpUGreaterThan"     },
			{SpvOpCode::OpSGreaterThan,      "OpSGreaterThan"     },
			{SpvOpCode::OpUGreaterThanEqual, "OpUGreaterThanEqual"},
			{SpvOpCode::OpSGreaterThanEqual, "OpSGreaterThanEqual"},
			{SpvOpCode::OpULessThan,         "OpULessThan"        },
			{SpvOpCode::OpSLessThan,         "OpSLessThan"        },
			{SpvOpCode::OpULessThanEqual,    "OpULessThanEqual"   },
			{SpvOpCode::OpSLessThanEqual,    "OpSLessThanEqual"   },
			// Control-flow instructions.
			{SpvOpCode::OpLabel,             "OpLabel"            },
			{SpvOpCode::OpReturn,            "OpReturn"           },
		};

		std::string_view SpvOpCodeToString(SpvOpCode opCode) {
			auto searchRes = spvOpCodeMap.find(opCode);
			assert(searchRes != spvOpCodeMap.end() &&
				"Couldn't find the instruction! Check the OpCode passed to the function!");
			if (searchRes == spvOpCodeMap.end()) {
				return "Undefined";
			}
			return searchRes->second;
		}

	}
}