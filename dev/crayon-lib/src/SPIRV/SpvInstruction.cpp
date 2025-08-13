#include "SPIRV/SpvInstruction.h"

namespace crayon {
	namespace spirv {
		Instruction::Instruction(SpvOpCode opCode, uint16_t wordCount)
			: resultId(0), resultType(0), opCode(opCode), wordCount(wordCount) {
		}
		Instruction::Instruction(SpvOpCode opCode, uint16_t wordCount, uint32_t resultType, uint32_t resultId)
			: resultId(resultId), resultType(resultType), opCode(opCode), wordCount(wordCount) {
		}

		void Instruction::PushOperand(uint32_t operand) {
			operands.push_back(operand);
		}

		const std::vector<uint32_t>& Instruction::GetOperands() const {
			return operands;
		}

		SpvOpCode Instruction::GetOpCode() const {
			return opCode;
		}

		uint32_t Instruction::GetResultType() const {
			return resultType;
		}

		uint32_t Instruction::GetResultId() const {
			return resultId;
		}

		Instruction OpDecorate(uint32_t variable, uint32_t decoration) {
			// TODO: based on the decoration provided, figure out how many additional
			// operands the decoration takes (must be non-id operands!).
			uint32_t extraOperandsCount = 0;
			uint32_t wordCount = 3 + extraOperandsCount;
			uint32_t resultType = 0;
			// TODO: generate a unique result id.
			uint32_t resultId = 0;
			Instruction opDecorate(SpvOpCode::OpDecorate, wordCount, resultType, resultId);
			return opDecorate;
		}

		Instruction OpTypeVoid() {
			return Instruction();
		}
		Instruction OpTypeFunction(uint32_t retType) {
			return Instruction();
		}
		Instruction OpTypeFunction(uint32_t retType, const std::vector<uint32_t> params) {
			return Instruction();
		}
		Instruction OpTypeFloat(uint32_t width) {
			return Instruction();
		}
		Instruction OpTypeVector(uint32_t type, uint32_t count) {
			return Instruction();
		}
		Instruction OpTypePointer(uint32_t storage, uint32_t type) {
			return Instruction();
		}
		Instruction OpVariable(uint32_t type, uint32_t storage) {
			return Instruction();
		}
		Instruction OpConstant(uint32_t type, int value) {
			return Instruction();
		}
		Instruction OpConstant(uint32_t type, uint32_t value) {
			return Instruction();
		}
		Instruction OpConstant(uint32_t type, float value) {
			return Instruction();
		}

	}
}