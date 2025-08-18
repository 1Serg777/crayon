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
		SpvInstruction OpTypeFloat(uint32_t width) {
			SpvInstruction opTypeFloat(SpvOpCode::OpTypeFloat, 3, spvIdGenerator.GenerateUniqueId());
			opTypeFloat.PushLiteralOperand(width);
			return opTypeFloat;
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

		SpvInstruction OpStore(const SpvInstruction& pointer, const SpvInstruction& object) {
			SpvInstruction opStore(SpvOpCode::OpStore, 3);
			opStore.PushIdOperand(pointer.GetResultId());
			opStore.PushIdOperand(object.GetResultId());
			return opStore;
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
			// 3. Result Type. (All result types are Ids?)
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
					}
					else {
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
						}
						else {
							out << operand.value;
						}
					}
				}
			}
		}

	}
}