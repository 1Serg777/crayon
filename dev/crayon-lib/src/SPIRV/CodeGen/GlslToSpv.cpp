#include "SPIRV/CodeGen/GlslToSpv.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace crayon {
	namespace spirv {

		using namespace glsl;

		SpvInstruction SpvEnvironment::GetTypeDeclInst(uint32_t typeId) const {
			auto pred = [=](const std::pair<std::string, SpvInstruction>& typeDeclPair) {
				return typeDeclPair.second.GetResultId() == typeId;
			};
			auto searchRes = std::find_if(typeDecls.begin(), typeDecls.end(), pred);
			assert(searchRes != typeDecls.end() && "Couldn't find the type declaration instruction!");
			if (searchRes == typeDecls.end())
				return SpvInstruction();
			return searchRes->second;
		}
		void SpvEnvironment::Clear() {
			typeDecls.clear();
			typePtrs.clear();
			functions.clear();
			varDecls.clear();
		}

		GlslToSpvGenerator::GlslToSpvGenerator(const GlslToSpvGeneratorConfig& config)
			: config(config) {
		}

		void GlslToSpvGenerator::CompileToSpv(glsl::ShaderProgramBlock* program) {
			// GenerateTestProgram();
			std::string_view name = program->GetShaderProgramName();
			shaderProgram.SetName(name);
			program->Accept(this);
		}

		const ShaderProgram& GlslToSpvGenerator::GetShaderProgram() const {
			return shaderProgram;
		}

		void GlslToSpvGenerator::GenerateTestProgram() {
			// SPIR-V instructions.
			SpvInstruction nop = OpNop();
			instructions.push_back(nop);
			// 1. SPIR-V mode-setting instructions.
			SpvInstruction shaderCapability = OpCapability(SpvCapability::SHADER);
			modeInstructions.push_back(shaderCapability);
			SpvInstruction opExtInstImport = OpExtInstImport(std::string_view{"GLSL.std.450"});
			modeInstructions.push_back(opExtInstImport);
			SpvInstruction opMemoryModel = OpMemoryModel(SpvAddressingModel::LOGICAL, SpvMemoryModel::GLSL_450);
			modeInstructions.push_back(opMemoryModel);
			// 2. OpTypeXXX
			// 2.1 OpTypeVoid
			SpvInstruction opTypeVoid = OpTypeVoid();
			tvc.push_back(opTypeVoid);
			// 2.2 OpTypeFloat
			SpvInstruction opTypeFloat = OpTypeFloat(32);
			tvc.push_back(opTypeFloat);
			// 2.3 OpTypeVector
			// Declaring a vec4 type.
			SpvInstruction opTypeVector3 = OpTypeVector(opTypeFloat, 3);
			tvc.push_back(opTypeVector3);
			SpvInstruction opTypeVector4 = OpTypeVector(opTypeFloat, 4);
			tvc.push_back(opTypeVector4);
			// Creating a type pointer to the vec4 type with the uniform storage class.
			SpvInstruction opTypePointerVec4 = OpTypePointer(opTypeVector3, SpvStorageClass::UNIFORM);
			tvc.push_back(opTypePointerVec4);
			// 3. Constant instructions.
			SpvInstruction opConstantFl_0_5 = OpConstant(opTypeFloat, 0.5f);
			tvc.push_back(opConstantFl_0_5);
			SpvInstruction opConstantFl_1_0 = OpConstant(opTypeFloat, 1.0f);
			tvc.push_back(opConstantFl_1_0);
			// 3.1 Composite constant (vec4)
			std::vector<SpvInstruction> constituents(4);
			constituents[0] = opConstantFl_0_5;
			constituents[1] = opConstantFl_0_5;
			constituents[2] = opConstantFl_0_5;
			constituents[3] = opConstantFl_1_0;
			SpvInstruction opConstantCompositeVec4 = OpConstantComposite(opTypeVector4, constituents);
			tvc.push_back(opConstantCompositeVec4);
			// 4. Functions
			// Creating a function type of the form: "void fun_name()" where "fun_name" can be any function name.
			// Function has no parameters and doesn't return anything.
			SpvInstruction opTypeFunctionVoid = OpTypeFunction(opTypeVoid);
			tvc.push_back(opTypeFunctionVoid);
			// Creating a void function with no parameters.
			SpvInstruction opVoidFunction = OpFunction(opTypeFunctionVoid, SpvFunctionControl::NONE);
			instructions.push_back(opVoidFunction);
			SpvInstruction voidFunBlockStart = OpLabel();
			instructions.push_back(voidFunBlockStart);
			// 5. Variable declaration.
			SpvInstruction vec3InTypePointer = OpTypePointer(opTypeVector3, SpvStorageClass::INPUT);
			tvc.push_back(vec3InTypePointer);
			SpvInstruction vec4OutTypePointer = OpTypePointer(opTypeVector4, SpvStorageClass::OUTPUT);
			tvc.push_back(vec4OutTypePointer);
			// a) Variable without an initializer.
			SpvInstruction vec3InVarDecl = OpVariable(vec3InTypePointer, SpvStorageClass::INPUT);
			SpvInstruction vec4OutVarDecl = OpVariable(vec4OutTypePointer, SpvStorageClass::OUTPUT);
			// b) Variable with an initializer.
			// SpvInstruction vec4VarDecl = OpVariable(vec4TypePointer, SpvStorageClass::OUTPUT, opConstantCompositeVec4);
			interfaceVars.push_back(vec3InVarDecl);
			interfaceVars.push_back(vec4OutVarDecl);
			// 6. Storing composite constant in a variable (whose types should match).
			// The type of the object must match the variable's type pointer type.
			SpvInstruction opStore = OpStore(vec4OutVarDecl, opConstantCompositeVec4);
			instructions.push_back(opStore);
			SpvInstruction funRet = OpReturn();
			instructions.push_back(funRet);
			SpvInstruction funEnd = OpFunctionEnd();
			instructions.push_back(funEnd);

			// 7. Decorations
			SpvInstruction inLocationDecoration = OpDecorateLocation(vec3InVarDecl, 0);
			decorations.push_back(inLocationDecoration);
			SpvInstruction outLocationDecoration = OpDecorateLocation(vec4OutVarDecl, 0);
			decorations.push_back(outLocationDecoration);

			// 8. Entry point instruction
			this->entryPointInst = OpEntryPoint(SpvExecutionModel::FRAGMENT, opVoidFunction,
				                                "main", interfaceVars);
		}

		void GlslToSpvGenerator::ClearState() {
			spvEnv.Clear();

			decorations.clear();
			tvc.clear();

			interfaceVars.clear();

			instructions.clear();
		}

		std::vector<uint32_t> GlslToSpvGenerator::GenerateSpvBinary() {
			std::vector<uint32_t> spvBinary;
			// TODO
			return spvBinary;
		}
		std::string GlslToSpvGenerator::GenerateSpvAsmText() {
			std::stringstream spvAsmText;
			idFieldWidth = CalcDigitCount(GetLastGeneratedSpvId()) + 1; // 1 is from the "%" character

			PrintExtInstructions(spvAsmText);
			PrintModeInstructions(spvAsmText);
			PrintEntryPointInstruction(spvAsmText);

			PrintDecorationInstructions(spvAsmText);
			PrintInstructions(spvAsmText, tvc);

			PrintFunctionInstructions(spvAsmText);

			return spvAsmText.str();
		}

		// NEW

		void GlslToSpvGenerator::CreateVertexInputLayoutInstructions() {
			const std::vector<std::shared_ptr<VertexAttribDecl>>& attribDecls = vertexInputLayoutBlock->GetAttribDecls();
			for (size_t i = 0; i < attribDecls.size(); i++) {
				const Token& channelToken = attribDecls[i]->GetChannel();
				VertexAttribChannel vertexAttribChannel = IdentifierTokenToVertexAttribChannel(channelToken);
				int location = GetVertexAttribChannelNum(vertexAttribChannel);

				SpvInstruction typePtrInst = GetTypePtrDeclInst(attribDecls[i]->GetTypeSpec(), SpvStorageClass::INPUT);
				SpvInstruction varDeclInst = OpVariable(typePtrInst, SpvStorageClass::INPUT);
				interfaceVars.push_back(varDeclInst);
				tvc.push_back(varDeclInst);

				SpvInstruction locDecInst = OpDecorateLocation(varDeclInst, static_cast<uint32_t>(location));
				decorations.push_back(locDecInst);
			}
		}
		void GlslToSpvGenerator::CreateColorAttachmentInstructions() {
			const std::vector<std::shared_ptr<ColorAttachmentDecl>>& colorAttachments =
				colorAttachmentsBlock->GetColorAttachments();

			for (size_t i = 0; i < colorAttachments.size(); i++) {
				const Token& channelToken = colorAttachments[i]->GetChannel();
				ColorAttachmentChannel colorAttachmentChannel = IdentifierTokenToColorAttachmentChannel(channelToken);
				int location = GetColorAttachmentChannelNum(colorAttachmentChannel);

				SpvInstruction typePtrInst = GetTypePtrDeclInst(colorAttachments[i]->GetTypeSpec(), SpvStorageClass::OUTPUT);
				SpvInstruction varDeclInst = OpVariable(typePtrInst, SpvStorageClass::OUTPUT);
				interfaceVars.push_back(varDeclInst);
				tvc.push_back(varDeclInst);

				SpvInstruction locDecInst = OpDecorateLocation(varDeclInst, static_cast<uint32_t>(location));
				decorations.push_back(locDecInst);
			}
		}

		SpvInstruction GlslToSpvGenerator::GetTypeDeclInst(const glsl::TypeSpec& typeSpec) {
			std::string mangledTypeName = MangleTypeName(typeSpec);
			auto searchRes = spvEnv.typeDecls.find(mangledTypeName);
			if (searchRes == spvEnv.typeDecls.end()) {
				SpvInstruction typeDeclInst = CreateTypeDeclInst(typeSpec);
				return typeDeclInst;
			}
			return searchRes->second;
		}
		SpvInstruction GlslToSpvGenerator::GetTypePtrDeclInst(const glsl::TypeSpec& typeSpec, SpvStorageClass storageClass) {
			std::string mangledTypePtrName = MangleTypePtrName(typeSpec, storageClass);
			auto searchRes = spvEnv.typePtrs.find(mangledTypePtrName);
			if (searchRes == spvEnv.typePtrs.end()) {
				SpvInstruction typePtrInst = CreateTypePtrDeclInst(typeSpec, storageClass);
				return typePtrInst;
			}
			return searchRes->second;
		}

		SpvInstruction GlslToSpvGenerator::CreateTypeDeclInst(const glsl::TypeSpec& typeSpec) {
			SpvInstruction typeDeclInst;

			if (typeSpec.IsArray()) {
				typeDeclInst = CreateArrayTypeDeclInst(typeSpec);
			} else {
				if (typeSpec.type.tokenType == TokenType::VOID) {
					typeDeclInst = OpTypeVoid();
				} else if (typeSpec.IsScalar()) {
					typeDeclInst = CreateScalarTypeDeclInst(typeSpec);
				} else if (typeSpec.IsVector()) {
					typeDeclInst = CreateVectorTypeDeclInst(typeSpec);
				} else if (typeSpec.IsMatrix()) {
					typeDeclInst = CreateMatrixTypeDeclInst(typeSpec);
				} else if (typeSpec.IsStructure()) {
					typeDeclInst = CreateStructureTypeDeclInst(typeSpec);
				}
			}

			std::string mangledTypeName = MangleTypeName(typeSpec);
			spvEnv.typeDecls.insert({mangledTypeName, typeDeclInst});
			tvc.push_back(typeDeclInst);
			return typeDeclInst;
		}
		SpvInstruction GlslToSpvGenerator::CreateTypePtrDeclInst(const glsl::TypeSpec& typeSpec, SpvStorageClass storageClass) {
			SpvInstruction typeDeclInst = GetTypeDeclInst(typeSpec);
			SpvInstruction typePtrInst = OpTypePointer(typeDeclInst, storageClass);
			std::string mangledTypePtrName = MangleTypePtrName(typeSpec, storageClass);
			spvEnv.typePtrs.insert({mangledTypePtrName, typePtrInst});
			tvc.push_back(typePtrInst);
			return typePtrInst;
		}

		SpvInstruction GlslToSpvGenerator::CreateScalarTypeDeclInst(const glsl::TypeSpec& typeSpec) {
			switch (typeSpec.type.tokenType) {
				case TokenType::BOOL: {
					return OpTypeBool();
				}
				case TokenType::INT: {
					return OpTypeInt(32, SpvSignedness::SIGNED);
				}
				case TokenType::UINT: {
					return OpTypeInt(32, SpvSignedness::UNSIGNED);
				}
				case TokenType::FLOAT: {
					return OpTypeFloat(32);
				}
				case TokenType::DOUBLE: {
					return OpTypeFloat(64);
				}
				default: {
					assert(false && "Non-scalar type provided!");
					return SpvInstruction();
				}
			}
		}
		SpvInstruction GlslToSpvGenerator::CreateVectorTypeDeclInst(const glsl::TypeSpec& typeSpec) {
			GlslBasicType glslVecType = TokenTypeToGlslBasicType(typeSpec.type.tokenType);
			GlslBasicType fundGlslType = GetFundamentalType(glslVecType);
			
			TypeSpec fundTypeSpec{};
			TokenType fundTokenType = GlslBasicTypeToTokenType(fundGlslType);
			fundTypeSpec.type = GenerateToken(fundTokenType);

			SpvInstruction fundTypeDeclInst = GetTypeDeclInst(fundTypeSpec);
			uint32_t componentCount = static_cast<uint32_t>(GetColVecNumberOfRows(glslVecType));
			return OpTypeVector(fundTypeDeclInst, componentCount);
		}
		SpvInstruction GlslToSpvGenerator::CreateMatrixTypeDeclInst(const glsl::TypeSpec& typeSpec) {
			// TODO
			return SpvInstruction();
		}
		SpvInstruction GlslToSpvGenerator::CreateStructureTypeDeclInst(const glsl::TypeSpec& typeSpec) {
			// TODO
			return SpvInstruction();
		}
		SpvInstruction GlslToSpvGenerator::CreateArrayTypeDeclInst(const glsl::TypeSpec& typeSpec) {
			// 1. A one-dimensional array (i.e., float [3], or vec4[2])
			if (typeSpec.dimensions.size() == 1) {
				TypeSpec baseTypeSpec{};
				baseTypeSpec.type = typeSpec.type;

				SpvInstruction baseTypeDeclInst = GetTypeDeclInst(baseTypeSpec);
				SpvInstruction dimConst = GetConstInst(static_cast<uint32_t>(typeSpec.dimensions[0].dimSize));
				SpvInstruction arrayDeclInst = OpTypeArray(baseTypeDeclInst, dimConst);
				return arrayDeclInst;
			}
			// 2. Arrays of more than one dimension.
			//    Use recursive calls to the GetTypeDeclInst method to progressively
			//    Obtain (get or create) array types of necessary dimensions.
			TypeSpec arrayType{};
			arrayType.type = typeSpec.type;
			arrayType.dimensions.assign(typeSpec.dimensions.begin() + 1, typeSpec.dimensions.end());

			SpvInstruction arrayTypeDeclInst = GetTypeDeclInst(arrayType);
			SpvInstruction dimConst = GetConstInst(static_cast<uint32_t>(typeSpec.dimensions[0].dimSize));
			SpvInstruction arrayDeclInst = OpTypeArray(arrayTypeDeclInst, dimConst);

			return arrayDeclInst;
		}

		// NEW

		SpvInstruction GlslToSpvGenerator::GetTypeFunDeclInst(const glsl::FunProto* funProto) {
			std::string mangledFunTypeName = MangleTypeFunctionName(funProto);
			auto searchRes = spvEnv.typeDecls.find(mangledFunTypeName);
			if (searchRes == spvEnv.typeDecls.end()) {
				// Create the requested type.
				SpvInstruction typeFunDeclInst = CreateTypeFunDeclInst(funProto);
				return typeFunDeclInst;
			}
			return searchRes->second;
		}
		SpvInstruction GlslToSpvGenerator::CreateTypeFunDeclInst(const glsl::FunProto* funProto) {
			const FullSpecType& retType = funProto->GetReturnType();
			SpvInstruction retTypeDeclInst = GetTypeDeclInst(retType.specifier);

			SpvInstruction typeFunDeclInst = OpTypeFunction(retTypeDeclInst);
			std::string mangledFunTypeName = MangleTypeFunctionName(funProto);
			spvEnv.typeDecls.insert({mangledFunTypeName, typeFunDeclInst});
			tvc.push_back(typeFunDeclInst);
			return typeFunDeclInst;
		}

		SpvInstruction GlslToSpvGenerator::CreateConstInst(int constVal) {
			TypeSpec typeSpec{};
			typeSpec.type = GenerateToken(TokenType::INT);
			SpvInstruction typeDeclInst = GetTypeDeclInst(typeSpec);
			SpvInstruction constDeclInst = OpConstant(typeDeclInst, constVal);

			std::string mangledConstName = MangleConstName(constVal);
			spvEnv.constants.insert({ mangledConstName, constDeclInst });
			tvc.push_back(constDeclInst);

			return constDeclInst;
		}
		SpvInstruction GlslToSpvGenerator::CreateConstInst(unsigned int constVal) {
			TypeSpec typeSpec{};
			typeSpec.type = GenerateToken(TokenType::UINT);
			SpvInstruction typeDeclInst = GetTypeDeclInst(typeSpec);
			SpvInstruction constDeclInst = OpConstant(typeDeclInst, constVal);

			std::string mangledConstName = MangleConstName(constVal);
			spvEnv.constants.insert({ mangledConstName, constDeclInst });
			tvc.push_back(constDeclInst);

			return constDeclInst;
		}
		SpvInstruction GlslToSpvGenerator::CreateConstInst(float constVal) {
			TypeSpec typeSpec{};
			typeSpec.type = GenerateToken(TokenType::FLOAT);
			SpvInstruction typeDeclInst = GetTypeDeclInst(typeSpec);
			SpvInstruction constDeclInst = OpConstant(typeDeclInst, constVal);

			std::string mangledConstName = MangleConstName(constVal);
			spvEnv.constants.insert({ mangledConstName, constDeclInst });
			tvc.push_back(constDeclInst);

			return constDeclInst;
		}
		SpvInstruction GlslToSpvGenerator::CreateConstInst(double constVal) {
			TypeSpec typeSpec{};
			typeSpec.type = GenerateToken(TokenType::DOUBLE);
			SpvInstruction typeDeclInst = GetTypeDeclInst(typeSpec);
			SpvInstruction constDeclInst = OpConstant(typeDeclInst, constVal);

			std::string mangledConstName = MangleConstName(constVal);
			spvEnv.constants.insert({ mangledConstName, constDeclInst });
			tvc.push_back(constDeclInst);

			return constDeclInst;
		}

		void GlslToSpvGenerator::PrintExtInstructions(std::ostream& out) const {
			PrintInstructions(out, extInstructions);
		}
		void GlslToSpvGenerator::PrintModeInstructions(std::ostream& out) const {
			PrintInstructions(out, modeInstructions);
		}
		void GlslToSpvGenerator::PrintEntryPointInstruction(std::ostream& out) const {
			size_t spvInstructionShift = idFieldWidth + 3; // 3 is from the " = " sequence of characters
			std::fill_n(
				std::ostream_iterator<char>(out),
				spvInstructionShift, ' ');
			// 1. OpCode Name.
			out << SpvOpCodeToString(entryPointInst.GetOpCode());
			// 2. Operands.
			const std::vector<uint32_t>& ops = entryPointInst.GetRawOperands();
			// 2.1 The first two instructions are the execution model and the id of the function representing the entry point.
			//     They are a literal and an identifier, respectively.
			out << " " << ops[0] << " %" << ops[1];
			// 2.2 Then we have a literal string representing the name of the function.
			//     Now, to print it, we need to be careful about how many words belong to the said name.
			//     We know where the string starts, but it's a bit problematic to get where it ends.
			//     We can use the number of "in" and "out" variables to find out how many words from the end of the
			//     instruction's operands we shouldn't "skip" or avoid touching.
			size_t strWordLen = ops.size() - 2 - interfaceVars.size(); // 2 comes from the first two operands.
			const char* entryPointName = reinterpret_cast<const char*>(&ops[2]);
			std::string_view entryPointNameView(entryPointName, strWordLen * sizeof(uint32_t));
			out << " \"" << entryPointNameView << "\"";
			// 3. Now we print the identifiers of the interface variables.
			//    First, we need to figure out the index of the first such operand.
			size_t intOpsIdxStart = 2 + strWordLen;
			for (size_t i = intOpsIdxStart; i < ops.size(); i++) {
				out << " %" << ops[i];
			}
			out << "\n";
		}

		void GlslToSpvGenerator::PrintDecorationInstructions(std::ostream& out) const {
			PrintInstructions(out, decorations);
		}
		void GlslToSpvGenerator::PrintTypeInstructions(std::ostream& out) const {
			// PrintInstructions(out, typeDeclInsts);
		}
		void GlslToSpvGenerator::PrintTypePtrInstructions(std::ostream& out) const {
			// PrintInstructions(out, typePtrInsts);
		}
		void GlslToSpvGenerator::PrintConstantInstructions(std::ostream& out) const {
			// PrintInstructions(out, constants);
		}
		void GlslToSpvGenerator::PrintFunctionInstructions(std::ostream& out) const {
			PrintInstructions(out, instructions);
		}

		void GlslToSpvGenerator::PrintInstructions(std::ostream& out, const std::vector<SpvInstruction>& instructions) const {
			for (const SpvInstruction& spvInstruction : instructions) {
				if (spvInstruction.HasResultId()) {
					out << std::setw(idFieldWidth);
				} else {
					size_t spvInstructionShift = idFieldWidth + 3; // 3 is from the " = " sequence of characters
					std::fill_n(
						std::ostream_iterator<char>(out),
						spvInstructionShift, ' ');
				}
				// PrintSpvInstruction(out, spvInstruction);
				PrintSpvInstructionAsmText(out, spvInstruction);
				out << "\n";
			}
		}

		void GlslToSpvGenerator::PrintSpvInstructionAsmText(std::ostream& out, const SpvInstruction& spvInstruction) const {
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
							// Based on the result type we can see
							// if the value should be interpreted as a float, int, or uint.
							uint32_t resType = spvInstruction.GetResultType();
							SpvInstruction constTypeDeclInst = spvEnv.GetTypeDeclInst(resType);
							SpvOpCode typeOpCode = constTypeDeclInst.GetOpCode();
							if (typeOpCode == SpvOpCode::OpTypeInt) {
								// Assume, for now, that we're dealing with 32-bit integers only.
								const std::vector<SpvInstOperand>& ops = constTypeDeclInst.GetOperands();
								// 1. First operand is the width literal, and
								// 2. The second operand is the signedness literal (0 - unsigned, 1 - signed)
								SpvSignedness signedness = static_cast<SpvSignedness>(ops[1].value);
								if (signedness == SpvSignedness::UNSIGNED) {
									unsigned int uint_val = *reinterpret_cast<const unsigned int*>(&operand.value);
									out << uint_val;
								} else if (signedness == SpvSignedness::SIGNED) {
									int int_val = *reinterpret_cast<const int*>(&operand.value);
									out << int_val;
								} else {
									assert(false && "Unsupported signedness provided!");
								}
							} else if (typeOpCode == SpvOpCode::OpTypeFloat) {
								// Assume, for now, that we're dealing with 32-bit floats only.
								const std::vector<SpvInstOperand>& ops = constTypeDeclInst.GetOperands();
								out << std::fixed << std::showpoint;
								out << std::setprecision(1);
								float fl_val = *reinterpret_cast<const float*>(&operand.value);
								out << fl_val;
							} else {
								assert(false && "Unsupported type provided!");
							}
						} else {
							out << operand.value;
						}
					}
				}
			}
		}
		void GlslToSpvGenerator::PrintSpvInstructionBinary(std::ostream& out, const SpvInstruction& spvInstruction) const {
			// TODO
		}

		void GlslToSpvGenerator::VisitShaderProgramBlock(glsl::ShaderProgramBlock* programBlock) {
			SpvInstruction shaderCapability = OpCapability(SpvCapability::SHADER);
			modeInstructions.push_back(shaderCapability);
			SpvInstruction opExtInstImport = OpExtInstImport(std::string_view{"GLSL.std.450"});
			modeInstructions.push_back(opExtInstImport);
			SpvInstruction opMemoryModel = OpMemoryModel(SpvAddressingModel::LOGICAL, SpvMemoryModel::GLSL_450);
			modeInstructions.push_back(opMemoryModel);
			for (const std::shared_ptr<Block>& block : programBlock->GetBlocks()) {
				block->Accept(this);
			}
		}
		void GlslToSpvGenerator::VisitFixedStagesConfigBlock(glsl::FixedStagesConfigBlock* fixedStagesConfigBlock) {
			// TODO
		}
		void GlslToSpvGenerator::VisitMaterialPropertiesBlock(glsl::MaterialPropertiesBlock* materialPropertiesBlock) {
			MaterialProps matPropsDesc{};
			matPropsDesc.name = ExtractStringLiteral(materialPropertiesBlock->GetName());
			for (const std::shared_ptr<MatPropDecl>& matPropDecl : materialPropertiesBlock->GetMatPropDecls()) {
				const Token& type = matPropDecl->GetType();
				const Token& name = matPropDecl->GetName();
				TokenType tokenType = MapMaterialPropertyType(type.tokenType);

				MaterialPropDesc matPropDesc{};
				matPropDesc.name = name.lexeme;
				// matPropDesc.visibleName; // TODO
				matPropDesc.type = TokenTypeToMaterialPropertyType(tokenType);
				// matPropDesc.showInEditor; // TODO

				matPropsDesc.AddMatProp(matPropDesc);
			}
			// TODO: more than one MaterialProperties block is allowed!
			shaderProgram.SetMaterialProps(matPropsDesc);
		}
		void GlslToSpvGenerator::VisitVertexInputLayoutBlock(glsl::VertexInputLayoutBlock* vertexInputLayoutBlock) {
			VertexInputLayoutDesc vertexInputLayoutDesc = GenerateVertexInputLayoutDesc(vertexInputLayoutBlock);
			shaderProgram.SetVertexInputLayout(vertexInputLayoutDesc);
			this->vertexInputLayoutBlock = vertexInputLayoutBlock;
		}
		void GlslToSpvGenerator::VisitColorAttachmentsBlock(glsl::ColorAttachmentsBlock* colorAttachmentsBlock) {
			// TODO
			this->colorAttachmentsBlock = colorAttachmentsBlock;
		}
		void GlslToSpvGenerator::VisitShaderBlock(glsl::ShaderBlock* shaderBlock) {
			ClearState();
			ShaderType shaderType = shaderBlock->GetShaderType();
			switch (shaderType) {
				case ShaderType::VS: {
					spvEnv.execModel = SpvExecutionModel::VERTEX;

					std::shared_ptr<InterfaceBlockDecl> glPerVertex = CreatePerVertexIntBlockDecl();
					glPerVertex->Accept(this);

					CreateVertexInputLayoutInstructions();
					
					std::shared_ptr<TransUnit> transUnit = shaderBlock->GetTranslationUnit();
					transUnit->Accept(this);

					if (config.type == SpvType::ASM) {
						shaderProgram.SetShaderModuleSpvAsm(ShaderType::VS, GenerateSpvAsmText());
					} else if (config.type == SpvType::BINARY) {
						shaderProgram.SetShaderModuleSpvBinary(ShaderType::VS, GenerateSpvBinary());
					}
					// ClearState();
					break;
				}
				case ShaderType::TCS: {
					// TODO
					break;
				}
				case ShaderType::TES: {
					// TODO
					break;
				}
				case ShaderType::GS: {
					// TODO
					break;
				}
				case ShaderType::FS: {
					spvEnv.execModel = SpvExecutionModel::FRAGMENT;

					CreateColorAttachmentInstructions();

					std::shared_ptr<TransUnit> transUnit = shaderBlock->GetTranslationUnit();
					transUnit->Accept(this);

					if (config.type == SpvType::ASM) {
						shaderProgram.SetShaderModuleSpvAsm(ShaderType::FS, GenerateSpvAsmText());
					} else if (config.type == SpvType::BINARY) {
						shaderProgram.SetShaderModuleSpvBinary(ShaderType::FS, GenerateSpvBinary());
					}
					break;
				}
				default: {
					assert(false && "Unidentifier shader stage provided!");
					return;
				}
			}
		}

		void GlslToSpvGenerator::VisitTransUnit(glsl::TransUnit* transUnit) {
			for (const std::shared_ptr<Decl>& decl : transUnit->GetDeclarations()) {
				decl->Accept(this);
			}
		}
		void GlslToSpvGenerator::VisitStructDecl(glsl::StructDecl* structDecl) {
			// TODO
		}
		void GlslToSpvGenerator::VisitInterfaceBlockDecl(glsl::InterfaceBlockDecl* intBlockDecl) {
			std::stringstream nameMangler;
			nameMangler << intBlockDecl->GetName().lexeme;
			// 1. First we create a struct type.
			std::vector<SpvInstruction> intBlockMembers(intBlockDecl->GetFieldCount());
			const std::vector<std::shared_ptr<glsl::VarDecl>>& intBlockFields = intBlockDecl->GetFields();
			for (size_t i = 0; i < intBlockFields.size(); i++) {
				// Every field must have a type, but not necessarily a type pointer.
				// A type pointer is only needed if we access a block member.
				// But this is going to be handled at the time of access.
				TypeSpec varTypeSpec = intBlockFields[i]->GetVarType().specifier;
				// Now's the time to handle variable and type array dimensions.
				const std::vector<ArrayDim>& varDimensions = intBlockFields[i]->GetDimensions();
				varTypeSpec.dimensions.insert(varTypeSpec.dimensions.begin(),
					                          varDimensions.begin(), varDimensions.end());
				SpvInstruction memberTypeDeclInst = GetTypeDeclInst(varTypeSpec);
				intBlockMembers[i] = memberTypeDeclInst;
			}
			// There's no need to try to get a type declaration instruction because we assume
			// that the parser has already made sure that an interface block is only declared once.
			SpvInstruction intBlockTypeDeclInst = OpTypeStruct(intBlockMembers);
			tvc.push_back(intBlockTypeDeclInst);
			spvEnv.typeDecls.insert({nameMangler.str(), intBlockTypeDeclInst});

			// Reset the name mangler.
			nameMangler.str("");

			// 2. Then we create a type pointer.
			const TypeQual& intBlockQual = intBlockDecl->GetTypeQualifier();
			SpvStorageClass spvStorageClass{};
			// The check isn't really needed since it's mandatory for an interface block to have a storage qualifier, right?
			// Again, we assume that the parser has already checked that.
			if (intBlockQual.storage.has_value()) {
				const Token& storageQual = intBlockQual.storage.value();
				// Only the following three storage qualifier types are allowed, right?
				switch (storageQual.tokenType) {
					case TokenType::IN:
						spvStorageClass = SpvStorageClass::INPUT;
						break;
					case TokenType::OUT:
						spvStorageClass = SpvStorageClass::OUTPUT;
						break;
					case TokenType::UNIFORM:
						spvStorageClass = SpvStorageClass::UNIFORM;
						break;
				}
			}
			nameMangler << "type_ptr_" << SpvStorageClassToString(spvStorageClass) << "_" << intBlockDecl->GetName().lexeme;
			SpvInstruction intBlockTypePtrInst = OpTypePointer(intBlockTypeDeclInst, spvStorageClass);
			tvc.push_back(intBlockTypePtrInst);
			spvEnv.typePtrs.insert({nameMangler.str(), intBlockTypePtrInst});

			// Reset the name mangler.
			nameMangler.str("");

			// 4. We also create a variable.
			nameMangler << "var_" << intBlockDecl->GetName().lexeme;
			SpvInstruction intBlockVarDeclInst = OpVariable(intBlockTypePtrInst, spvStorageClass);
			tvc.push_back(intBlockVarDeclInst);
			spvEnv.varDecls.insert({nameMangler.str(), intBlockVarDeclInst});

			// 5. And finally we need to decorate it with a Buffer decoration to denote
			//    that the structure type we've created establishes a memory interface block.
			SpvInstruction intBlockDecoration = OpDecorateStructTypeIntBlock(intBlockTypeDeclInst);
			decorations.push_back(intBlockDecoration);

			// But that's not all. If this interface block is one of the predefined ones,
			// we'll have to decorate its members with a BuiltIn decoration.
			// Until I figure out a better way of handling that, I'm just simply going to
			// hardcode it here for the "gl_PerVertex" interface block (the only built-in interface block we're going to use).

			if (intBlockDecl->GetName().lexeme == "gl_PerVertex") {
				SpvInstruction glPositionDecoration = OpDecorateMemberBuiltIn(intBlockTypeDeclInst, 0, SpvBuiltIn::POSITION);
				SpvInstruction pointSizeDecoration = OpDecorateMemberBuiltIn(intBlockTypeDeclInst, 1, SpvBuiltIn::POINT_SIZE);
				SpvInstruction clipDistanceDecoration = OpDecorateMemberBuiltIn(intBlockTypeDeclInst, 2, SpvBuiltIn::CLIP_DISTANCE);
				SpvInstruction cullDistanceDecoration = OpDecorateMemberBuiltIn(intBlockTypeDeclInst, 3, SpvBuiltIn::CULL_DISTANCE);
				decorations.push_back(glPositionDecoration);
				decorations.push_back(pointSizeDecoration);
				decorations.push_back(clipDistanceDecoration);
				decorations.push_back(cullDistanceDecoration);
			}
		}
		void GlslToSpvGenerator::VisitDeclList(glsl::DeclList* declList) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFunDecl(glsl::FunDecl* funDecl) {
			if (funDecl->IsFunDecl())
				return;

			// 2. Function prototype.
			std::shared_ptr<FunProto> funProto = funDecl->GetFunProto();
			std::string funMangledName = MangleTypeFunctionName(funProto.get());
			SpvInstruction typeFunDeclInst = GetTypeFunDeclInst(funProto.get());

			SpvInstruction funDeclInst = OpFunction(typeFunDeclInst, SpvFunctionControl::NONE);
			instructions.push_back(funDeclInst);

			const Token& funName = funProto->GetFunctionName();
			if (funName.lexeme == entryPointFunName) {
				// Create an entry point instruction.
				entryPointInst = OpEntryPoint(spvEnv.execModel, funDeclInst, entryPointFunName, interfaceVars);
			}

			std::shared_ptr<BlockStmt> funStmts = funDecl->GetBlockStmt();
			funStmts->Accept(this);
			// VisitBlockStmt(funStmts.get());

			const FullSpecType& retType = funProto->GetReturnType();
			GlslBasicType glslRetType = TokenTypeToGlslBasicType(retType.specifier.type.tokenType);
			if (glslRetType == glsl::GlslBasicType::VOID) {
				SpvInstruction funRetInst = OpReturn();
				instructions.push_back(funRetInst);
			}
			SpvInstruction funEndInst = OpFunctionEnd();
			instructions.push_back(funEndInst);
		}
		void GlslToSpvGenerator::VisitQualDecl(glsl::QualDecl* qualDecl) {
			// TODO
		}
		void GlslToSpvGenerator::VisitVarDecl(glsl::VarDecl* varDecl) {
			const FullSpecType& varType = varDecl->GetVarType();
			const Token& identifier = varDecl->GetVarName();

			SpvStorageClass storageClass{SpvStorageClass::PRIVATE}; // Global variable without qualifiers.
			if (spvEnv.scopeCtx == SpvScopeContext::EXTERNAL) {
				if (varType.qualifier.storage.has_value()) {
					switch(varType.qualifier.storage.value().tokenType) {
						case TokenType::IN:
							storageClass = SpvStorageClass::INPUT;
							break;
						case TokenType::OUT:
							storageClass = SpvStorageClass::OUTPUT;
							break;
						case TokenType::UNIFORM:
							storageClass = SpvStorageClass::UNIFORM;
							break;
						case TokenType::BUFFER:
							storageClass = SpvStorageClass::STORAGE_BUFFER;
							break;
						default:
							assert(false && "Unrecognized storage qualifier provided!");
							return;
					}
				}
			} else if (spvEnv.scopeCtx == SpvScopeContext::FUNCTION ||
					   spvEnv.scopeCtx == SpvScopeContext::BLOCK) {
				storageClass = SpvStorageClass::FUNCTION;
			}

			// Both type and type pointer creation, if necessary, will be handled with this call.
			SpvInstruction typePtrInst = GetTypePtrDeclInst(varType.specifier, storageClass);

			SpvInstruction varDeclInst = OpVariable(typePtrInst, storageClass);
			if (storageClass == SpvStorageClass::INPUT ||
				storageClass == SpvStorageClass::OUTPUT) {
				interfaceVars.push_back(varDeclInst);
			}
			tvc.push_back(varDeclInst);

			if (!varType.qualifier.layout.empty()) {
				int location{-1};
				for (const glsl::LayoutQualifier& layoutQual : varType.qualifier.layout) {
					if (layoutQual.name.lexeme == "location" &&
						layoutQual.value.has_value()) {
						location = layoutQual.value.value();
					}
				}
				if (location != -1) {
					SpvInstruction locDecInst = OpDecorateLocation(varDeclInst, static_cast<uint32_t>(location));
					decorations.push_back(locDecInst);
				}
			}
		}

		void GlslToSpvGenerator::VisitBlockStmt(glsl::BlockStmt* blockStmt) {
			// Every block starts with a label.
			SpvInstruction labelInst = OpLabel();
			instructions.push_back(labelInst);
			// TODO:
		}
		void GlslToSpvGenerator::VisitDeclStmt(glsl::DeclStmt* declStmt) {
			// TODO
		}
		void GlslToSpvGenerator::VisitExprStmt(glsl::ExprStmt* exprStmt) {
			// TODO
		}

		void GlslToSpvGenerator::VisitInitListExpr(glsl::InitListExpr* initListExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitAssignExpr(glsl::AssignExpr* assignExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitBinaryExpr(glsl::BinaryExpr* binaryExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitUnaryExpr(glsl::UnaryExpr* unaryExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFieldSelectExpr(glsl::FieldSelectExpr* fieldSelectExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFunCallExpr(glsl::FunCallExpr* funCallExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitCtorCallExpr(glsl::CtorCallExpr* ctorCallExpr) {
			// Check to see if all of the constructor's parameters are constant.
			// 1. If so, then we'll need an OpConstantComposite instruction,
			// 2. Otherwise, it'll be an OpCompositeConstruct instruction.
			// TODO
		}
		void GlslToSpvGenerator::VisitVarExpr(glsl::VarExpr* varExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitIntConstExpr(glsl::IntConstExpr* intConstExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitUintConstExpr(glsl::UintConstExpr* uintConstExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitFloatConstExpr(glsl::FloatConstExpr* floatConstExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitDoubleConstExpr(glsl::DoubleConstExpr* doubleConstExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitGroupExpr(glsl::GroupExpr* groupExpr) {
			// TODO
		}

		// NEW

		std::string MangleTypeName(const glsl::TypeSpec& typeSpec) {
			std::stringstream nameMangler;
			nameMangler << typeSpec.type.lexeme;
			if (typeSpec.IsArray()) {
				for (size_t i = 0; i < typeSpec.dimensions.size(); i++) {
					nameMangler << "_" << MangleConstName(static_cast<unsigned int>(typeSpec.dimensions[i].dimSize));
				}
			}
			return nameMangler.str();
		}
		std::string MangleTypePtrName(const glsl::TypeSpec& typeSpec, SpvStorageClass storageClass) {
			std::stringstream nameMangler;
			nameMangler << "type_ptr_";
			nameMangler << SpvStorageClassToString(storageClass) << "_";
			nameMangler << MangleTypeName(typeSpec);
			return nameMangler.str();
		}

		// NEW

		std::string MangleTypeName(const glsl::VarDecl* varDecl) {
			const FullSpecType& varType = varDecl->GetVarType();
			// GlslBasicType glslBasicType = TokenTypeToGlslBasicType(varType.specifier.type.tokenType);
			std::stringstream nameMangler;
			nameMangler << varType.specifier.type.lexeme;
			if (varDecl->IsArray()) {
				const std::vector<ArrayDim>& varTypeDims = varType.specifier.dimensions;
				const std::vector<ArrayDim>& varDims = varDecl->GetDimensions();
				for (size_t i = 0; i < varDims.size(); i++) {
					nameMangler << MangleConstName(static_cast<unsigned int>(varDims[i].dimSize));
				}
				for (size_t i = 0; i < varTypeDims.size(); i++) {
					nameMangler << MangleConstName(static_cast<unsigned int>(varTypeDims[i].dimSize));
				}
			}
			return nameMangler.str();
		}

		std::string MangleConstName(int intConst) {
			std::stringstream nameMangler;
			nameMangler << TokenTypeToLexeme(TokenType::INT) << "_";
			nameMangler << intConst;
			return nameMangler.str();
		}
		std::string MangleConstName(unsigned int uintConst) {
			std::stringstream nameMangler;
			nameMangler << TokenTypeToLexeme(TokenType::UINT) << "_";
			nameMangler << uintConst;
			return nameMangler.str();
		}
		std::string MangleConstName(float floatConst) {
			std::stringstream nameMangler;
			nameMangler << TokenTypeToLexeme(TokenType::FLOAT) << "_";
			nameMangler << std::fixed << std::showpoint;
			nameMangler << floatConst;
			return nameMangler.str();
		}
		std::string MangleConstName(double doubleConst) {
			std::stringstream nameMangler;
			nameMangler << TokenTypeToLexeme(TokenType::DOUBLE) << "_";
			nameMangler << std::fixed << std::showpoint;
			nameMangler << doubleConst;
			return nameMangler.str();
		}

		std::string MangleTypeFunctionName(const glsl::FunProto* funProto) {
			std::stringstream nameMangler;
			// 1. Return type.
			const FullSpecType& retType = funProto->GetReturnType();
			nameMangler << MangleTypeName(retType.specifier);
			nameMangler << "_type_fun_";
			// 2. Parameter list.
			if (!funProto->FunParamListEmpty()) {
				// TODO: figure out what to do with the parameters.
				const std::vector<std::shared_ptr<FunParam>>& funParamList = funProto->GetFunParamList();
				for (const std::shared_ptr<FunParam>& funParam : funParamList) {
					const FullSpecType& paramType = funParam->GetVarType();
					// TODO
					if (funParam->HasName()) {
						const Token& identifier = funParam->GetVarName();
						// TODO
					}
				}
			}
			return nameMangler.str();
		}

		std::string MangleTypeName(glsl::VertexAttribDecl* vertexAttrib) {
			const TypeSpec& typeSpec = vertexAttrib->GetTypeSpec();
			return MangleTypeName(typeSpec);
		}
		std::string MangleTypeName(glsl::MatPropDecl* matProp) {
			// TODO
			return std::string();
		}
		std::string MangleTypeName(glsl::ColorAttachmentDecl* colorAttachment) {
			const TypeSpec& typeSpec = colorAttachment->GetTypeSpec();
			return MangleTypeName(typeSpec);
		}

		std::string MangleTypePointerName(glsl::VertexAttribDecl* vertexAttrib) {
			const TypeSpec& typeSpec = vertexAttrib->GetTypeSpec();
			SpvStorageClass storageClass = SpvStorageClass::OUTPUT;
			return MangleTypePtrName(typeSpec, storageClass);
		}
		std::string MangleTypePointerName(glsl::MatPropDecl* matProp) {
			// TODO
			return std::string();
		}
		std::string MangleTypePointerName(glsl::ColorAttachmentDecl* colorAttachment) {
			const TypeSpec& typeSpec = colorAttachment->GetTypeSpec();
			SpvStorageClass storageClass = SpvStorageClass::OUTPUT;
			return MangleTypePtrName(typeSpec, storageClass);
		}

	}
}