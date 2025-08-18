#pragma once

#include "SpvCommon.h"

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace crayon {
	namespace spirv {

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
			LOCATION = 30,
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
			INPUT    = 1,
			UNIFORM  = 2,
			OUTPUT   = 3,
			PRIVATE  = 6,
			FUNCTION = 7,
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

		class SpvInstruction {
		public:
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

		private:
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

		// Variable declaration instructions.

		SpvInstruction OpDecorateLocation(uint32_t variable, uint32_t location);
		SpvInstruction OpDecorateLocation(const SpvInstruction& varDeclInst, uint32_t location);

		SpvInstruction OpTypeVoid();
		SpvInstruction OpTypeFunction(uint32_t retType);
		SpvInstruction OpTypeFunction(const SpvInstruction& retTypeDeclInst);
		SpvInstruction OpTypeFunction(uint32_t retType, const std::vector<uint32_t> params);
		SpvInstruction OpTypeFunction(const SpvInstruction& typeDeclInst, const std::vector<uint32_t> params);
		SpvInstruction OpTypeFloat(uint32_t width);
		SpvInstruction OpTypeVector(uint32_t type, uint32_t count);
		SpvInstruction OpTypeVector(const SpvInstruction& typeDeclInst, uint32_t count);
		SpvInstruction OpTypePointer(uint32_t type, SpvStorageClass storageClass);
		SpvInstruction OpTypePointer(const SpvInstruction& typeDeclInst, SpvStorageClass storageClass);

		SpvInstruction OpVariable(const SpvInstruction& typePointer, SpvStorageClass storageClass);
		SpvInstruction OpVariable(const SpvInstruction& typePointer, SpvStorageClass storageClass,
			                      const SpvInstruction& initializer);

		// Constant instructions.

		SpvInstruction OpConstant(uint32_t type, int value);
		SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, int value);
		SpvInstruction OpConstant(uint32_t type, uint32_t value);
		SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, uint32_t value);
		SpvInstruction OpConstant(uint32_t type, float value);
		SpvInstruction OpConstant(const SpvInstruction& typeDeclInst, float value);

		SpvInstruction OpConstantComposite(const SpvInstruction& typeDeclInst,
			                               const std::vector<SpvInstruction>& constituents);

		// Function instructions.

		SpvInstruction OpFunction(const SpvInstruction& typeFunctionInst, SpvFunctionControl funCtrl);
		SpvInstruction OpFunctionEnd();

		// Memory instructions.

		SpvInstruction OpStore(const SpvInstruction& pointer, const SpvInstruction& object);

		// Control-flow instructions.

		SpvInstruction OpLabel();
		SpvInstruction OpReturn();

		std::string SpvInstructionToString(const SpvInstruction& spvInstruction);
		void PrintSpvInstruction(std::ostream& out, const SpvInstruction& spvInstruction);

	}
}