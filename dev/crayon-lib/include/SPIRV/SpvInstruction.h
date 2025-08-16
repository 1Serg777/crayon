#pragma once

#include "SpvCommon.h"

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace crayon {
	namespace spirv {

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
			void PushIdOperand(uint32_t operand);
			bool HasOperands() const;
			const std::vector<SpvInstOperand>& GetOperands() const;

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
			SpvOpCode opCode;
		};

		uint32_t GetLastGeneratedSpvId();

		SpvInstruction OpNop();

		// Variable declaration instructions.

		SpvInstruction OpDecorate(uint32_t variable, uint32_t decoration);

		SpvInstruction OpTypeVoid();
		SpvInstruction OpTypeFunction(uint32_t retType);
		SpvInstruction OpTypeFunction(uint32_t retType, const std::vector<uint32_t> params);
		SpvInstruction OpTypeFloat(uint32_t width);
		SpvInstruction OpTypeVector(uint32_t type, uint32_t count);
		SpvInstruction OpTypeVector(const SpvInstruction& typeDeclInstruction, uint32_t count);
		SpvInstruction OpTypePointer(uint32_t storage, uint32_t type);
		SpvInstruction OpVariable(uint32_t type, uint32_t storage);

		SpvInstruction OpConstant(uint32_t type, int value);
		SpvInstruction OpConstant(uint32_t type, uint32_t value);
		SpvInstruction OpConstant(uint32_t type, float value);

		std::string SpvInstructionToString(const SpvInstruction& spvInstruction);
		void PrintSpvInstruction(std::ostream& out, const SpvInstruction& spvInstruction);

	}
}