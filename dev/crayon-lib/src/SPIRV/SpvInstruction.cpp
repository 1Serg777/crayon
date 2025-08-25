#include "SPIRV/SpvInstruction.h"

#include "Utility.h"

#include <cassert>
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace crayon {
	namespace spirv {

		static SeqIdGenerator<uint32_t> spvIdGenerator;

		static const std::unordered_map<SpvOpCode, std::string_view> spvOpCodeToStrMap = {
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
			auto searchRes = spvOpCodeToStrMap.find(opCode);
			assert(searchRes != spvOpCodeToStrMap.end() &&
				"Couldn't find the instruction! Check the OpCode passed to the function!");
			if (searchRes == spvOpCodeToStrMap.end()) {
				return "";
			}
			return searchRes->second;
		}

		static const std::unordered_map<SpvStorageClass, std::string_view> spvStorageClassToStrMap = {
			{SpvStorageClass::UNIFORM_CONSTANT, "UniformConstant"},
			{SpvStorageClass::INPUT,            "Input"          },
			{SpvStorageClass::UNIFORM,          "Uniform"        },
			{SpvStorageClass::OUTPUT,           "Output"         },
			{SpvStorageClass::WORKGROUP,        "Workgroup"      },
			{SpvStorageClass::CROSS_WORKGROUP,  "CrossWorkgroup" },
			{SpvStorageClass::PRIVATE,          "Private"        },
			{SpvStorageClass::FUNCTION,         "Function"       },
			{SpvStorageClass::PUSH_CONSTANT,    "PushConstant"   },
			{SpvStorageClass::IMAGE,            "Image"          },
			{SpvStorageClass::STORAGE_BUFFER,   "StorageBuffer"  },
		};

		std::string_view SpvStorageClassToString(SpvStorageClass spvStorageClass) {
			auto searchRes = spvStorageClassToStrMap.find(spvStorageClass);
			assert(searchRes != spvStorageClassToStrMap.end() &&
				"Couldn't find the storage class provided! Check the SpvStorageClass passed to the function!");
			if (searchRes == spvStorageClassToStrMap.end()) {
				return "";
			}
			return searchRes->second;
		}

		SpvInstruction::SpvInstruction()
			: SpvInstruction(SpvOpCode::OpNop, 0) {
		}
		SpvInstruction::SpvInstruction(SpvOpCode opCode, uint16_t wordCount)
			: opCode(opCode), wordCount(wordCount) {
		}
		SpvInstruction::SpvInstruction(SpvOpCode opCode, uint16_t wordCount, uint32_t resultId)
			: resultId(resultId), opCode(opCode), wordCount(wordCount) {
		}
		SpvInstruction::SpvInstruction(SpvOpCode opCode, uint16_t wordCount, uint32_t resultId, uint32_t resultType)
			: resultId(resultId), resultType(resultType), opCode(opCode), wordCount(wordCount) {
		}

		void SpvInstruction::PushLiteralOperand(uint32_t operand) {
			SpvInstOperand spvInstOperand{SpvInstOperandType::LITERAL, operand};
			instOps.push_back(spvInstOperand);
		}
		void SpvInstruction::PushLiteralOperands(const std::vector<uint32_t>& operands) {
			for (uint32_t opLiteral : operands) {
				SpvInstOperand operand{SpvInstOperandType::LITERAL, opLiteral};
				instOps.push_back(operand);
			}
		}
		void SpvInstruction::PushLiteralOperands(const std::vector<SpvInstOperand>& operands) {
			for (const SpvInstOperand& operand : operands) {
				assert(operand.operandType == SpvInstOperandType::LITERAL && "The operand must be a literal!");
				instOps.push_back(operand);
			}
		}
		void SpvInstruction::PushIdOperand(uint32_t operand) {
			SpvInstOperand spvInstOperand{SpvInstOperandType::ID, operand};
			instOps.push_back(spvInstOperand);
		}
		void SpvInstruction::PushIdOperands(const std::vector<uint32_t>& operands) {
			for (uint32_t opId : operands) {
				SpvInstOperand operand{SpvInstOperandType::ID, opId};
				instOps.push_back(operand);
			}
		}
		void SpvInstruction::PushIdOperands(const std::vector<SpvInstOperand>& operands) {
			for (const SpvInstOperand& operand : operands) {
				assert(operand.operandType == SpvInstOperandType::ID && "The operand must be an id!");
				instOps.push_back(operand);
			}
		}

		bool SpvInstruction::HasOperands() const {
			return instOps.size() != 0;
		}
		const std::vector<SpvInstOperand>& SpvInstruction::GetOperands() const {
			return instOps;
		}
		std::vector<uint32_t> SpvInstruction::GetRawOperands() const {
			std::vector<uint32_t> rawOperands(instOps.size());
			for (size_t i = 0; i < instOps.size(); i++) {
				rawOperands[i] = instOps[i].value;
			}
			return rawOperands;
		}
		bool SpvInstruction::HasOperand(size_t idx) const {
			return idx < instOps.size();
		}
		const SpvInstOperand& SpvInstruction::GetOperand(size_t idx) const {
			assert(HasOperand(idx) && "Check if the operand exists first!");
			return instOps[idx];
		}

		size_t SpvInstruction::GetOperandCount() const {
			return instOps.size();
		}

		SpvOpCode SpvInstruction::GetOpCode() const {
			return opCode;
		}

		bool SpvInstruction::HasResultId() const {
			return resultId != 0;
		}
		uint32_t SpvInstruction::GetResultId() const {
			// assert(HasResultId() && "Check if the instruction has result id!");
			return resultId;
		}
		
		bool SpvInstruction::HasResultType() const {
			return resultType != 0;
		}
		uint32_t SpvInstruction::GetResultType() const {
			assert(HasResultType() && "Check if the instruction has result type!");
			return resultType;
		}

		uint32_t GetLastGeneratedSpvId() {
			return spvIdGenerator.GetLastGeneratedUniqueId();
		}

		std::vector<uint32_t> GenerateLiteralStringWords(std::string_view str) {
			uint16_t strWordLen = static_cast<uint16_t>(str.size() / sizeof(uint32_t));
			if (str.size() % sizeof(uint32_t) != 0)
				strWordLen++;
			std::vector<uint32_t> strWords(strWordLen);
			std::memset(strWords.data(), 0, strWordLen * sizeof(uint32_t));
			std::memcpy(strWords.data(), str.data(), str.size() * sizeof(char));
			return strWords;
		}

		SpvInstruction OpNop() {
			SpvInstruction opNop(SpvOpCode::OpNop, 1);
			return opNop;
		}

		SpvInstruction OpMemoryModel(SpvAddressingModel addressingModel, SpvMemoryModel memoryModel) {
			SpvInstruction opMemoryModel(SpvOpCode::OpMemoryModel, 3);
			opMemoryModel.PushLiteralOperand(static_cast<uint32_t>(addressingModel));
			opMemoryModel.PushLiteralOperand(static_cast<uint32_t>(memoryModel));
			return opMemoryModel;
		}
		SpvInstruction OpEntryPoint(SpvExecutionModel execModel,
			                        const SpvInstruction& funInst,
			                        std::string_view entryPointName,
			                        const std::vector<SpvInstruction>& interfaceVars) {
			std::vector<uint32_t> entryPointNameWords = GenerateLiteralStringWords(entryPointName);
			uint16_t wordCount = static_cast<uint16_t>(3 + entryPointNameWords.size() + interfaceVars.size());
			SpvInstruction opEntryPoint(SpvOpCode::OpEntryPoint, wordCount);
			opEntryPoint.PushLiteralOperand(static_cast<uint32_t>(execModel));
			opEntryPoint.PushIdOperand(funInst.GetResultId());
			opEntryPoint.PushLiteralOperands(entryPointNameWords);
			for (const SpvInstruction& interfaceVar : interfaceVars) {
				opEntryPoint.PushIdOperand(interfaceVar.GetResultId());
			}
			return opEntryPoint;
		}
		SpvInstruction OpCapability(SpvCapability capability) {
			SpvInstruction opCapability(SpvOpCode::OpCapability, 2);
			opCapability.PushLiteralOperand(static_cast<uint32_t>(capability));
			return opCapability;
		}
		SpvInstruction OpExtInstImport(std::string_view extInstSetName) {
			uint16_t nameWordLen = static_cast<uint16_t>(extInstSetName.size() / sizeof(uint32_t));
			if (extInstSetName.size() % sizeof(uint32_t) != 0)
				nameWordLen++;
			SpvInstruction opExtInstImport(SpvOpCode::OpExtInstImport, 2 + nameWordLen, spvIdGenerator.GenerateUniqueId());
			std::vector<uint32_t> nameWords(nameWordLen);
			std::memset(nameWords.data(), 0, nameWordLen * sizeof(uint32_t));
			std::memcpy(nameWords.data(), extInstSetName.data(), extInstSetName.size() * sizeof(char));
			opExtInstImport.PushLiteralOperands(nameWords);
			return opExtInstImport;
		}

		SpvInstruction OpDecorateStructTypeIntBlock(const SpvInstruction& structType) {
			SpvInstruction opDecorate(SpvOpCode::OpDecorate, 3);
			opDecorate.PushIdOperand(structType.GetResultId());
			opDecorate.PushLiteralOperand(static_cast<uint32_t>(SpvDecoration::BLOCK));
			return opDecorate;
		}

		SpvInstruction OpDecorateMemberBuiltIn(const SpvInstruction& structType, uint32_t member, SpvBuiltIn builtIn) {
			SpvInstruction opMemberDecorate(SpvOpCode::OpMemberDecorate, 5);
			opMemberDecorate.PushIdOperand(structType.GetResultId());
			opMemberDecorate.PushLiteralOperand(member);
			opMemberDecorate.PushLiteralOperand(static_cast<uint32_t>(SpvDecoration::BUILTIN));
			opMemberDecorate.PushLiteralOperand(static_cast<uint32_t>(builtIn));
			return opMemberDecorate;
		}

		SpvInstruction OpDecorateLocation(uint32_t variable, uint32_t location) {
			SpvInstruction opDecorate(SpvOpCode::OpDecorate, 4);
			opDecorate.PushIdOperand(variable);
			opDecorate.PushLiteralOperand(static_cast<uint32_t>(SpvDecoration::LOCATION));
			opDecorate.PushLiteralOperand(location);
			return opDecorate;
		}
		SpvInstruction OpDecorateLocation(const SpvInstruction& varDeclInst, uint32_t location) {
			SpvInstruction opDecorate(SpvOpCode::OpDecorate, 4);
			opDecorate.PushIdOperand(varDeclInst.GetResultId());
			opDecorate.PushLiteralOperand(static_cast<uint32_t>(SpvDecoration::LOCATION));
			opDecorate.PushLiteralOperand(location);
			return opDecorate;
		}

		SpvInstruction OpTypeVoid() {
			SpvInstruction opTypeVoid(SpvOpCode::OpTypeVoid, 2, spvIdGenerator.GenerateUniqueId());
			return opTypeVoid;
		}
		SpvInstruction OpTypeBool() {
			SpvInstruction opTypeBool(SpvOpCode::OpTypeBool, 2, spvIdGenerator.GenerateUniqueId());
			return opTypeBool;
		}
		SpvInstruction OpTypeInt(uint32_t width, SpvSignedness signedness) {
			SpvInstruction opTypeInt(SpvOpCode::OpTypeInt, 4, spvIdGenerator.GenerateUniqueId());
			opTypeInt.PushLiteralOperand(width);
			opTypeInt.PushLiteralOperand(static_cast<uint32_t>(signedness));
			return opTypeInt;
		}
		SpvInstruction OpTypeFloat(uint32_t width) {
			SpvInstruction opTypeFloat(SpvOpCode::OpTypeFloat, 3, spvIdGenerator.GenerateUniqueId());
			opTypeFloat.PushLiteralOperand(width);
			return opTypeFloat;
		}

		SpvInstruction OpTypeFunction(uint32_t retType) {
			SpvInstruction opTypeFunction(SpvOpCode::OpFunction, 3, spvIdGenerator.GenerateUniqueId());
			opTypeFunction.PushIdOperand(retType);
			return opTypeFunction;
		}
		SpvInstruction OpTypeFunction(const SpvInstruction& retTypeDeclInst) {
			SpvInstruction opTypeFunction(SpvOpCode::OpTypeFunction, 3, spvIdGenerator.GenerateUniqueId());
			opTypeFunction.PushIdOperand(retTypeDeclInst.GetResultId());
			return opTypeFunction;
		}
		SpvInstruction OpTypeFunction(uint32_t retType, const std::vector<uint32_t> params) {
			// TODO
			return SpvInstruction();
		}
		SpvInstruction OpTypeFunction(const SpvInstruction& typeDeclInst, const std::vector<uint32_t> params) {
			// TODO
			return SpvInstruction();
		}
		
		SpvInstruction OpTypeVector(uint32_t type, uint32_t count) {
			SpvInstruction opTypeVector(SpvOpCode::OpTypeVector, 4, spvIdGenerator.GenerateUniqueId());
			opTypeVector.PushIdOperand(type);
			opTypeVector.PushLiteralOperand(count);
			return opTypeVector;
		}
		SpvInstruction OpTypeVector(const SpvInstruction& typeDeclInst, uint32_t count) {
			SpvInstruction opTypeVector(SpvOpCode::OpTypeVector, 4, spvIdGenerator.GenerateUniqueId());
			opTypeVector.PushIdOperand(typeDeclInst.GetResultId());
			opTypeVector.PushLiteralOperand(count);
			return opTypeVector;
		}
		
		SpvInstruction OpTypeArray(const SpvInstruction& elementType, const SpvInstruction& lengthConstInst) {
			SpvInstruction opTypeArray(SpvOpCode::OpTypeArray, 4, spvIdGenerator.GenerateUniqueId());
			opTypeArray.PushIdOperand(elementType.GetResultId());
			opTypeArray.PushIdOperand(lengthConstInst.GetResultId());
			return opTypeArray;
		}
		SpvInstruction OpTypeStruct(const std::vector<SpvInstruction>& members) {
			uint16_t wordCount = static_cast<uint16_t>(2 + members.size());
			SpvInstruction opTypeStruct(SpvOpCode::OpTypeStruct, wordCount, spvIdGenerator.GenerateUniqueId());
			for (const SpvInstruction& member : members) {
				opTypeStruct.PushIdOperand(member.GetResultId());
			}
			return opTypeStruct;
		}

		SpvInstruction OpTypePointer(uint32_t type, SpvStorageClass storageClass) {
			SpvInstruction opTypePointer(SpvOpCode::OpTypePointer, 4, spvIdGenerator.GenerateUniqueId());
			opTypePointer.PushLiteralOperand(static_cast<uint32_t>(storageClass));
			opTypePointer.PushIdOperand(type);
			return opTypePointer;
		}
		SpvInstruction OpTypePointer(const SpvInstruction& typeDeclInst, SpvStorageClass storageClass) {
			SpvInstruction opTypePointer(SpvOpCode::OpTypePointer, 4, spvIdGenerator.GenerateUniqueId());
			opTypePointer.PushLiteralOperand(static_cast<uint32_t>(storageClass));
			opTypePointer.PushIdOperand(typeDeclInst.GetResultId());
			return opTypePointer;
		}

		SpvInstruction OpVariable(const SpvInstruction& typePointer, SpvStorageClass storageClass) {
			SpvInstruction opVariable(SpvOpCode::OpVariable, 4,
				                      spvIdGenerator.GenerateUniqueId(),
				                      typePointer.GetResultId());
			opVariable.PushLiteralOperand(static_cast<uint32_t>(storageClass));
			return opVariable;
		}
		SpvInstruction OpVariable(const SpvInstruction& typePointer, SpvStorageClass storageClass,
			                      const SpvInstruction& initializer) {
			SpvInstruction opVariable(SpvOpCode::OpVariable, 5,
				                      spvIdGenerator.GenerateUniqueId(),
				                      typePointer.GetResultId());
			opVariable.PushLiteralOperand(static_cast<uint32_t>(storageClass));
			opVariable.PushIdOperand(initializer.GetResultId());
			return opVariable;
		}

		SpvInstruction OpLoad(const SpvInstruction& type, const SpvInstruction& pointer) {
			SpvInstruction opLoad(SpvOpCode::OpLoad, 4, spvIdGenerator.GenerateUniqueId(), type.GetResultId());
			opLoad.PushIdOperand(pointer.GetResultId());
			return opLoad;
		}
		SpvInstruction OpStore(const SpvInstruction& pointer, const SpvInstruction& object) {
			SpvInstruction opStore(SpvOpCode::OpStore, 3);
			opStore.PushIdOperand(pointer.GetResultId());
			opStore.PushIdOperand(object.GetResultId());
			return opStore;
		}

		SpvInstruction OpAccessChain(const SpvInstruction& resultTypePtr,
			                         const SpvInstruction& baseType,
			                         const SpvInstruction& fieldIdxConst) {
			SpvInstruction opAccessChain(SpvOpCode::OpAccessChain, 5,
				                         spvIdGenerator.GenerateUniqueId(),
				                         resultTypePtr.GetResultId());
			opAccessChain.PushIdOperand(baseType.GetResultId());
			opAccessChain.PushIdOperand(fieldIdxConst.GetResultId());
			return opAccessChain;
		}

		/*
		SpvInstruction OpConstant(uint32_t type, int value) {
			SpvInstruction opConstant(SpvOpCode::OpConstant, 4,
				                      spvIdGenerator.GenerateUniqueId(),
				                      type);
			// Will the sign be preserved?
			// How do we even handle signed constants?
			// An unsigned constant + the unary "-" operation?
			uint32_t u32_val = *reinterpret_cast<const uint32_t*>(&value);
			opConstant.PushLiteralOperand(u32_val);
			return opConstant;
		}
		SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, int value) {
			// TODO
			return SpvInstruction();
		}
		SpvInstruction OpConstant(uint32_t type, uint32_t value) {
			return SpvInstruction();
		}
		SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, uint32_t value) {
			// TODO
			return SpvInstruction();
		}
		SpvInstruction OpConstant(uint32_t type, float value) {
			SpvInstruction opConstant(SpvOpCode::OpConstant, 4,
				                      spvIdGenerator.GenerateUniqueId(),
				                      type);
			uint32_t u32_val = *reinterpret_cast<const uint32_t*>(&value);
			opConstant.PushLiteralOperand(u32_val);
			return opConstant;
		}
		SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, float value) {
			SpvInstruction opConstant(SpvOpCode::OpConstant, 4,
				                      spvIdGenerator.GenerateUniqueId(),
				                      typeDeclInst.GetResultId());
			uint32_t u32_val = *reinterpret_cast<const uint32_t*>(&value);
			opConstant.PushLiteralOperand(u32_val);
			return opConstant;
		}
		SpvInstruction OpConstant(uint32_t type, double value) {
			SpvInstruction opConstant(SpvOpCode::OpConstant, 4,
				                      spvIdGenerator.GenerateUniqueId(),
				                      type);
			uint32_t u32_val = *reinterpret_cast<const uint32_t*>(&value);
			opConstant.PushLiteralOperand(u32_val);
			return opConstant;
		}
		SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, double value) {
			SpvInstruction opConstant(SpvOpCode::OpConstant, 4,
				                      spvIdGenerator.GenerateUniqueId(),
				                      typeDeclInst.GetResultId());
			uint32_t u32_val = *reinterpret_cast<const uint32_t*>(&value);
			opConstant.PushLiteralOperand(u32_val);
			return opConstant;
		}
		*/
		
		SpvInstruction OpConstant(uint32_t type, void* valPtr) {
			SpvInstruction opConstant(SpvOpCode::OpConstant, 4,
				                      spvIdGenerator.GenerateUniqueId(),
				                      type);
			uint32_t u32_val = *reinterpret_cast<const uint32_t*>(valPtr);
			opConstant.PushLiteralOperand(u32_val);
			return opConstant;
		}

		SpvInstruction OpConstantComposite(const SpvInstruction& typeDeclInst,
			                               const std::vector<SpvInstruction>& constituents) {
			uint16_t wordCount = 3 + static_cast<uint16_t>(constituents.size());
			SpvInstruction opConstantComposite(SpvOpCode::OpConstantComposite,
				                               wordCount,
				                               spvIdGenerator.GenerateUniqueId(),
				                               typeDeclInst.GetResultId());
			for (const SpvInstruction& constituent : constituents) {
				opConstantComposite.PushIdOperand(constituent.GetResultId());
			}
			return opConstantComposite;
		}

		SpvInstruction OpFunction(const SpvInstruction& typeFunctionInst, SpvFunctionControl funCtrl) {
			const SpvInstOperand funRetTypeId = typeFunctionInst.GetOperand(0);
			SpvInstruction opFunction(SpvOpCode::OpFunction, 5,
				                      spvIdGenerator.GenerateUniqueId(),
				                      funRetTypeId.value);
			opFunction.PushLiteralOperand(static_cast<uint32_t>(funCtrl));
			opFunction.PushIdOperand(typeFunctionInst.GetResultId());
			return opFunction;
		}
		SpvInstruction OpFunctionEnd() {
			SpvInstruction funEnd(SpvOpCode::OpFunctionEnd, 1);
			return funEnd;
		}

		SpvInstruction OpLabel() {
			SpvInstruction opLabel(SpvOpCode::OpLabel, 2, spvIdGenerator.GenerateUniqueId());
			return opLabel;
		}
		SpvInstruction OpReturn() {
			SpvInstruction opReturn(SpvOpCode::OpReturn, 1);
			return opReturn;
		}

		std::string SpvInstructionToString(const SpvInstruction& spvInstruction) {
			std::stringstream out;
			PrintSpvInstruction(out, spvInstruction);
			return out.str();
		}
		void PrintSpvInstruction(std::ostream& out, const SpvInstruction& spvInstruction) {
			// 1. Result Id.
			if (spvInstruction.HasResultId()) {
				std::stringstream idOutput;
				idOutput << "%" << spvInstruction.GetResultId();
				out << idOutput.str() << " = ";
			}
			// 2. OpCode Name.
			out << SpvOpCodeToString(spvInstruction.GetOpCode());
			// 3. Result Type. (Are all result types Ids?)
			if (spvInstruction.HasResultType()) {
				out << " %" << spvInstruction.GetResultType();
			}
			// 4. Operands, if any.
			if (spvInstruction.HasOperands()) {
				if (spvInstruction.GetOpCode() == SpvOpCode::OpExtInstImport) {
					const std::vector<uint32_t>& operands = spvInstruction.GetRawOperands();
					size_t operandCount = operands.size();
					const char* extNamePtr = reinterpret_cast<const char*>(operands.data());
					std::string_view extNameView(extNamePtr, operandCount * sizeof(uint32_t));
					out << " \"" << extNameView << "\"";
					return;
				}
				for (const SpvInstOperand& operand : spvInstruction.GetOperands()) {
					out << " ";
					if (operand.operandType == SpvInstOperandType::ID) {
						out << "%" << operand.value;
					} else {
						if (spvInstruction.GetOpCode() == SpvOpCode::OpConstant) {
							uint32_t resType = spvInstruction.GetResultType();
							// Based on the result type we can see
							// if the value should be interpreted as a float, int, or uint.
							// For now, we assume the only constants we're going to be dealing with
							// are floating-point constants.
							out << std::fixed << std::showpoint;
							out << std::setprecision(1);
							float fl_val = *reinterpret_cast<const float*>(&operand.value);
							out << fl_val;
						} else {
							out << operand.value;
						}
					}
				}
			}
		}

	}
}