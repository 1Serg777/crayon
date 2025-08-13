#pragma once

#include "SpvCommon.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace crayon {
	namespace spirv {

		class Instruction {
		public:
			Instruction() = default;
			Instruction(SpvOpCode opCode, uint16_t wordCount);
			Instruction(SpvOpCode opCode, uint16_t wordCount, uint32_t resultType, uint32_t resultId);

			void PushOperand(uint32_t operand);
			const std::vector<uint32_t>& GetOperands() const;

			SpvOpCode GetOpCode() const;

			uint32_t GetResultType() const;
			uint32_t GetResultId() const;

		private:
			std::vector<uint32_t> operands;
			uint32_t resultId{0};
			uint32_t resultType{0};
			SpvOpCode opCode{SpvOpCode::OpNop};
			uint16_t wordCount{0};
		};

		// Variable declaration instructions.

		Instruction OpDecorate(uint32_t variable, uint32_t decoration);

		Instruction OpTypeVoid();
		Instruction OpTypeFunction(uint32_t retType);
		Instruction OpTypeFunction(uint32_t retType, const std::vector<uint32_t> params);
		Instruction OpTypeFloat(uint32_t width);
		Instruction OpTypeVector(uint32_t type, uint32_t count);
		Instruction OpTypePointer(uint32_t storage, uint32_t type);
		Instruction OpVariable(uint32_t type, uint32_t storage);

		Instruction OpConstant(uint32_t type, int value);
		Instruction OpConstant(uint32_t type, uint32_t value);
		Instruction OpConstant(uint32_t type, float value);

	}
}