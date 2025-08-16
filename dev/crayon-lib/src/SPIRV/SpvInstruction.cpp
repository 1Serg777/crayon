#include "SPIRV/SpvInstruction.h"

#include "Utility.h"

#include <cassert>
#include <iomanip>
#include <sstream>

namespace crayon {
	namespace spirv {

		static SeqIdGenerator<uint32_t> spvIdGenerator;

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
		void SpvInstruction::PushIdOperand(uint32_t operand) {
			SpvInstOperand spvInstOperand{SpvInstOperandType::ID, operand};
			instOps.push_back(spvInstOperand);
		}
		bool SpvInstruction::HasOperands() const {
			return instOps.size() != 0;
		}
		const std::vector<SpvInstOperand>& SpvInstruction::GetOperands() const {
			return instOps;
		}

		SpvOpCode SpvInstruction::GetOpCode() const {
			return opCode;
		}

		bool SpvInstruction::HasResultId() const {
			return resultId != 0;
		}
		uint32_t SpvInstruction::GetResultId() const {
			assert(HasResultId() && "Check if the instruction has result id!");
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

		SpvInstruction OpNop() {
			SpvInstruction opNop(SpvOpCode::OpNop, 1);
			return opNop;
		}

		SpvInstruction OpDecorate(uint32_t variable, uint32_t decoration) {
			// TODO: based on the decoration provided, figure out how many additional
			// operands the decoration takes (must be non-id operands!).
			uint32_t extraOperandsCount = 0;
			uint32_t wordCount = 3 + extraOperandsCount;
			uint32_t resultType = 0;
			// TODO: generate a unique result id.
			uint32_t resultId = 0;
			SpvInstruction opDecorate(SpvOpCode::OpDecorate, wordCount, resultId, resultType);
			return opDecorate;
		}

		SpvInstruction OpTypeVoid() {
			SpvInstruction opTypeVoid(SpvOpCode::OpTypeVoid, 2, spvIdGenerator.GenerateUniqueId());
			return opTypeVoid;
		}
		SpvInstruction OpTypeFunction(uint32_t retType) {
			return SpvInstruction();
		}
		SpvInstruction OpTypeFunction(uint32_t retType, const std::vector<uint32_t> params) {
			return SpvInstruction();
		}
		SpvInstruction OpTypeFloat(uint32_t width) {
			SpvInstruction opTypeFloat(SpvOpCode::OpTypeFloat, 3, spvIdGenerator.GenerateUniqueId());
			opTypeFloat.PushLiteralOperand(width);
			return opTypeFloat;
		}
		SpvInstruction OpTypeVector(uint32_t type, uint32_t count) {
			return SpvInstruction();
		}
		SpvInstruction OpTypeVector(const SpvInstruction& typeDeclInstruction, uint32_t count) {
			SpvInstruction opTypeVector(SpvOpCode::OpTypeVector, 4, spvIdGenerator.GenerateUniqueId());
			opTypeVector.PushIdOperand(typeDeclInstruction.GetResultId());
			opTypeVector.PushLiteralOperand(count);
			return opTypeVector;
		}
		SpvInstruction OpTypePointer(uint32_t storage, uint32_t type) {
			return SpvInstruction();
		}
		SpvInstruction OpVariable(uint32_t type, uint32_t storage) {
			return SpvInstruction();
		}
		SpvInstruction OpConstant(uint32_t type, int value) {
			return SpvInstruction();
		}
		SpvInstruction OpConstant(uint32_t type, uint32_t value) {
			return SpvInstruction();
		}
		SpvInstruction OpConstant(uint32_t type, float value) {
			return SpvInstruction();
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
			// 3. Operands, if any.
			if (spvInstruction.HasOperands()) {
				for (const SpvInstOperand& operand : spvInstruction.GetOperands()) {
					out << " ";
					if (operand.operandType == SpvInstOperandType::ID) {
						out << "%";
					}
					out << operand.value;
				}
			}
		}

	}
}