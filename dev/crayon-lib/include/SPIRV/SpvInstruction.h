#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace crayon {
	namespace spirv {

		enum class SpvType {
			ASM,
			BINARY,
		};

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
			OpExtInstImport     = 11,
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

		std::string_view SpvOpCodeToString(SpvOpCode opCode);

		enum class SpvExecutionModel {
			VERTEX = 0, // Shader capability must be enabled
			TESSELLATION_CONTROL = 1, // Tessellation capability must be enabled
			TESSELLATION_EVALUATION = 2, // Tessellation capability must be enabled
			GEOMETRY = 3, // Geometry capability must be enabled
			FRAGMENT = 4, // Shader capability must be enabled
			GL_COMPUTE = 5, // Shader capability must be enabled
			KERNEL = 6, // Kernel capability must be enabled
		};

		enum class SpvAddressingModel {
			LOGICAL = 0,
			PHYSICAL_32 = 1, // Addresses capability must be enabled
			PHYSICAL_64 = 2, // Addresses capability must be enabled
			PHYSICAL_STORAGE_BUFFER_64 = 5348, // PhysicalStorageBufferAddresses must be enabled
		};

		enum class SpvMemoryModel {
			SIMPLE = 0, // Deprecated, use GLSL_450
			GLSL_450 = 1, // Shader capability must be enabled
			OpenCL = 2, // Kernel capability must be enabled
			VULKAN = 3, // VulkanMemoryModel capability must be enabled
		};

		enum class SpvDecoration {
			BLOCK = 2,
			BUILTIN = 11,
			LOCATION = 30,
		};

		enum class SpvBuiltIn {
			POSITION       = 0,
			POINT_SIZE     = 1,
			CLIP_DISTANCE  = 3,
			CULL_DISTANCE  = 4,
			VERTEX_ID      = 5,
			INSTANCE_ID    = 6,
			PRIMITIVE_ID   = 7,
			FRAG_COORD     = 15,
			POINT_COORD    = 16,
			FRONT_FACING   = 17,
			SAMPLE_MASK    = 20,
			FRAG_DEPTH     = 22,
			NUM_WORKGROUPS = 24,
			WORKGROUP_SIZE = 25,
			WORKGROUP_ID   = 26,
		};

		enum class SpvCapability {
			MATRIX = 0,
			SHADER = 1,
			GEOMETRY = 2,
			TESSELLATION = 3,
		};

		enum class SpvFunctionControl {
			NONE       = 0x0,
			INLINE     = 0x1,
			DONTINLINE = 0x2,
			PURE       = 0x4,
			CONST      = 0x8,
		};

		enum class SpvStorageClass {
			UNIFORM_CONSTANT = 0,
			INPUT            = 1,
			UNIFORM          = 2,
			OUTPUT           = 3,
			WORKGROUP        = 4,
			CROSS_WORKGROUP  = 5,
			PRIVATE          = 6,
			FUNCTION         = 7,
			PUSH_CONSTANT    = 9,
			IMAGE            = 11,
			STORAGE_BUFFER   = 12,
		};

		std::string_view SpvStorageClassToString(SpvStorageClass spvStorageClass);

		enum class SpvSignedness {
			UNSIGNED = 0,
			SIGNED   = 1,
		};

		enum class SpvInstOperandType {
			UNDEFINED = -1,
			ID,
			LITERAL,
		};

		struct SpvInstOperand {
			SpvInstOperandType operandType{SpvInstOperandType::UNDEFINED};
			uint32_t value{0};
		};

		struct SpvInstruction {
			SpvInstruction();
			SpvInstruction(SpvOpCode opCode, uint16_t wordCount);
			SpvInstruction(SpvOpCode opCode, uint16_t wordCount, uint32_t resultId);
			SpvInstruction(SpvOpCode opCode, uint16_t wordCount, uint32_t resultId, uint32_t resultType);

			void PushLiteralOperand(uint32_t operand);
			void PushLiteralOperands(const std::vector<uint32_t>& operands);
			void PushLiteralOperands(const std::vector<SpvInstOperand>& operands);
			void PushIdOperand(uint32_t operand);
			void PushIdOperands(const std::vector<uint32_t>& operands);
			void PushIdOperands(const std::vector<SpvInstOperand>& operands);

			bool HasOperands() const;
			const std::vector<SpvInstOperand>& GetOperands() const;
			std::vector<uint32_t> GetRawOperands() const;
			bool HasOperand(size_t idx) const;
			const SpvInstOperand& GetOperand(size_t idx) const;

			size_t GetOperandCount() const;

			SpvOpCode GetOpCode() const;

			bool HasResultId() const;
			uint32_t GetResultId() const;
			bool HasResultType() const;
			uint32_t GetResultType() const;

			std::vector<SpvInstOperand> instOps;
			uint32_t resultId{0};
			uint32_t resultType{0};
			uint16_t wordCount{0};
			SpvOpCode opCode{SpvOpCode::OpNop};
		};

		uint32_t GetLastGeneratedSpvId();

		std::vector<uint32_t> GenerateLiteralStringWords(std::string_view str);

		// SPIR-V instructions.

		SpvInstruction OpNop();

		// SPIR-V mode-setting instructions.

		SpvInstruction OpMemoryModel(SpvAddressingModel addressingModel, SpvMemoryModel memoryModel);
		SpvInstruction OpEntryPoint(SpvExecutionModel execModel,
			                        const SpvInstruction& funInst,
			                        std::string_view entryPointName,
			                        const std::vector<SpvInstruction>& interfaceVars);
		SpvInstruction OpCapability(SpvCapability capability);
		SpvInstruction OpExtInstImport(std::string_view extInstSetName);

		// Annotation instructions.

		SpvInstruction OpDecorateStructTypeIntBlock(const SpvInstruction& structType);

		SpvInstruction OpDecorateMemberBuiltIn(const SpvInstruction& structType, uint32_t member, SpvBuiltIn builtIn);

		SpvInstruction OpDecorateLocation(uint32_t variable, uint32_t location);
		SpvInstruction OpDecorateLocation(const SpvInstruction& varDeclInst, uint32_t location);

		// Type-declaration instructions.

		SpvInstruction OpTypeVoid();
		SpvInstruction OpTypeBool();
		SpvInstruction OpTypeInt(uint32_t width, SpvSignedness signedness);
		SpvInstruction OpTypeFloat(uint32_t width);

		SpvInstruction OpTypeFunction(uint32_t retType);
		SpvInstruction OpTypeFunction(const SpvInstruction& retTypeDeclInst);
		SpvInstruction OpTypeFunction(uint32_t retType, const std::vector<uint32_t> params);
		SpvInstruction OpTypeFunction(const SpvInstruction& typeDeclInst, const std::vector<uint32_t> params);
		
		SpvInstruction OpTypeVector(uint32_t type, uint32_t count);
		SpvInstruction OpTypeVector(const SpvInstruction& typeDeclInst, uint32_t count);

		SpvInstruction OpTypeArray(const SpvInstruction& elementType, const SpvInstruction& lengthConstInst);
		SpvInstruction OpTypeStruct(const std::vector<SpvInstruction>& members);

		SpvInstruction OpTypePointer(uint32_t type, SpvStorageClass storageClass);
		SpvInstruction OpTypePointer(const SpvInstruction& typeDeclInst, SpvStorageClass storageClass);

		// Memory instructions.

		SpvInstruction OpVariable(const SpvInstruction& typePointer, SpvStorageClass storageClass);
		SpvInstruction OpVariable(const SpvInstruction& typePointer, SpvStorageClass storageClass,
			                      const SpvInstruction& initializer);

		SpvInstruction OpStore(const SpvInstruction& pointer, const SpvInstruction& object);

		// Constant instructions.

		//SpvInstruction OpConstant(uint32_t type, int value);
		//SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, int value);
		//SpvInstruction OpConstant(uint32_t type, uint32_t value);
		//SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, uint32_t value);
		//SpvInstruction OpConstant(uint32_t type, float value);
		//SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, float value);
		//SpvInstruction OpConstant(uint32_t type, double value);
		//SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, double value);

		SpvInstruction OpConstant(uint32_t type, void* valPtr);
		template <typename T>
		SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, T value) {
			return OpConstant(typeDeclInst.GetResultId(), &value);
		}

		SpvInstruction OpConstantComposite(const SpvInstruction& typeDeclInst,
			                               const std::vector<SpvInstruction>& constituents);

		// Function instructions.

		SpvInstruction OpFunction(const SpvInstruction& typeFunctionInst, SpvFunctionControl funCtrl);
		SpvInstruction OpFunctionEnd();

		// Control-flow instructions.

		SpvInstruction OpLabel();
		SpvInstruction OpReturn();

		std::string SpvInstructionToString(const SpvInstruction& spvInstruction);
		void PrintSpvInstruction(std::ostream& out, const SpvInstruction& spvInstruction);

	}
}