#include "SPIRV/CodeGen/GlslToSpv.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>

namespace crayon {
	namespace spirv {

		using namespace glsl;

		glsl::InterfaceBlockDecl* SpvEnvironment::GetIntBlock(std::string_view intBlockName) {
			auto intBlockSearchRes = intBlocks.find(intBlockName);
			assert(intBlockSearchRes != intBlocks.end() && "Check if the interface block exists first!");
			if (intBlockSearchRes == intBlocks.end())
				return nullptr;
			return intBlockSearchRes->second;
		}
		glsl::VarDecl* SpvEnvironment::GetIntBlockVarDecl(std::string_view intBlockName,
			                                                              std::string_view varName) {
			auto intBlockSearchRes = intBlocks.find(intBlockName);
			assert(intBlockSearchRes != intBlocks.end() && "Make sure that the interface block exists first!");
			if (intBlockSearchRes == intBlocks.end()) return nullptr;
			assert(intBlockSearchRes->second->HasField(varName) && "Check the existence of the field first!");
			return intBlockSearchRes->second->GetField(varName).get();
		}
		glsl::VarDecl* SpvEnvironment::GetIntBlockVarDecl(std::string_view intBlockName,
			                                                              std::string_view varName,
			                                                              size_t& fieldIdx) {
			auto intBlockSearchRes = intBlocks.find(intBlockName);
			assert(intBlockSearchRes != intBlocks.end() && "Make sure that the interface block exists first!");
			if (intBlockSearchRes == intBlocks.end()) return nullptr;
			assert(intBlockSearchRes->second->HasField(varName) && "Check the existence of the field first!");
			return intBlockSearchRes->second->GetField(varName, fieldIdx).get();
		}
		glsl::VarDecl* SpvEnvironment::GetVarDecl(std::string_view varName) {
			auto varDeclSearchRes = variables.find(varName);
			assert(varDeclSearchRes != variables.end() && "Check if the variable declaration exists first!");
			return varDeclSearchRes->second;
		}

		bool SpvEnvironment::HasIntBlockVarDecl(std::string_view intBlockName, std::string_view varName) {
			auto intBlockSearchRes = intBlocks.find(intBlockName);
			if (intBlockSearchRes == intBlocks.end())
				return false;
			return intBlockSearchRes->second->HasField(varName);
		}
		bool SpvEnvironment::HasIntBlock(std::string_view intBlockName, std::string_view varName) {
			auto intBlockSearchRes = intBlocks.find(intBlockName);
			return intBlockSearchRes != intBlocks.end();
		}
		bool SpvEnvironment::HasVarDecl(std::string_view varName) {
			auto varDeclSearchRes = variables.find(varName);
			return varDeclSearchRes != variables.end();
		}

		void SpvEnvironment::AddIntBlockDecl(glsl::InterfaceBlockDecl* intBlockDecl) {
			intBlocks.insert({intBlockDecl->GetName().lexeme, intBlockDecl});
		}
		void SpvEnvironment::AddVarDecl(glsl::VarDecl* varDecl) {
			variables.insert({varDecl->GetVarName().lexeme, varDecl});
		}

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
			intBlocks.clear();
			variables.clear();

			typeDecls.clear();
			typePtrs.clear();
			functions.clear();
			varDecls.clear();
			constants.clear();
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
			PrintFirstWords(spvBinary);
			PrintInstructions(spvBinary, extInstructions);
			PrintInstructions(spvBinary, modeInstructions);
			PrintInstruction(spvBinary, entryPointInst);
			PrintInstructions(spvBinary, decorations);
			PrintInstructions(spvBinary, instructions);
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
			spvEnv.vertexInputVarDecls = CreateVertexAttribDecls(shaderProgram.GetVertexInputLayout());
			for (const std::shared_ptr<VarDecl>& vertexAttribVarDecl : spvEnv.vertexInputVarDecls) {
				spvEnv.AddVarDecl(vertexAttribVarDecl.get());
			}

			const std::vector<std::shared_ptr<VertexAttribDecl>>& attribDecls = vertexInputLayoutBlock->GetAttribDecls();
			for (size_t i = 0; i < attribDecls.size(); i++) {
				const Token& channelToken = attribDecls[i]->GetChannel();
				VertexAttribChannel vertexAttribChannel = IdentifierTokenToVertexAttribChannel(channelToken);
				int location = GetVertexAttribChannelNum(vertexAttribChannel);

				SpvInstruction typePtrInst = GetTypePtrDeclInst(attribDecls[i]->GetTypeSpec(), SpvStorageClass::INPUT);
				SpvInstruction varDeclInst = OpVariable(typePtrInst, SpvStorageClass::INPUT);
				interfaceVars.push_back(varDeclInst);
				tvc.push_back(varDeclInst);

				const Token& varNameTok = attribDecls[i]->GetName();
				std::string varName = std::string{varNameTok.lexeme};
				spvEnv.varDecls.insert({varName, varDeclInst});

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

				const Token& varNameTok = colorAttachments[i]->GetName();
				std::string varName = std::string{varNameTok.lexeme};
				spvEnv.varDecls.insert({varName, varDeclInst});

				SpvInstruction locDecInst = OpDecorateLocation(varDeclInst, static_cast<uint32_t>(location));
				decorations.push_back(locDecInst);
			}

			spvEnv.colorAttachmentVarDecls = CreateColorAttachmentVarDecls(shaderProgram.GetColorAttachments());
			for (const std::shared_ptr<VarDecl>& colorAttachmentVarDecl : spvEnv.colorAttachmentVarDecls) {
				spvEnv.AddVarDecl(colorAttachmentVarDecl.get());
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
			TokenType fundType = GetFundamentalType(typeSpec.type.tokenType);
			
			TypeSpec fundTypeSpec{};
			TokenType fundTokenType = fundType;
			fundTypeSpec.type = GenerateToken(fundTokenType);

			SpvInstruction fundTypeDeclInst = GetTypeDeclInst(fundTypeSpec);
			uint32_t componentCount = static_cast<uint32_t>(GetColVecNumberOfRows(typeSpec.type.tokenType));
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

		SpvInstruction GlslToSpvGenerator::AccessIntBlockField(std::string_view intBlockName, std::string_view fieldName) {
			// Produce an OpAccessChain instruction.
			// 1. First we retrieve the interface block declaration.
			InterfaceBlockDecl* glPerVertex = spvEnv.GetIntBlock(intBlockName);
			// 2. Then we get the field declaration pointer as well as its index among other fields.
			size_t fieldIdx{ 0 };
			std::shared_ptr<VarDecl> fieldDecl = glPerVertex->GetField(fieldName, fieldIdx);
			// 3. Now we need a type pointer that is going to be used to construct an OpAccessChain instruction.
			//    So we basically combine the storage qualifier of the interface block and
			//    the type specifier of the field.
			const TypeSpec& fieldTypeSpec = fieldDecl->GetVarType().specifier;

			SpvStorageClass typePtrStorageClass{};
			const Token& storageQualTok = glPerVertex->GetTypeQualifier().storage.value();
			switch (storageQualTok.tokenType) {
				case TokenType::IN:
					typePtrStorageClass = SpvStorageClass::INPUT;
					break;
				case TokenType::OUT:
					typePtrStorageClass = SpvStorageClass::OUTPUT;
					break;
				case TokenType::UNIFORM:
					typePtrStorageClass = SpvStorageClass::UNIFORM;
					break;
			}
			SpvInstruction typePtrDeclInst = GetTypePtrDeclInst(fieldTypeSpec, typePtrStorageClass);

			// 4. Now we need the interface block declaration instruction.
			SpvInstruction intBlockVarDeclInst = spvEnv.varDecls.find(std::string{intBlockName})->second;

			// 5. And finally, we need to convert the field index into a constant instruction.
			SpvInstruction fieldIdxConstInst = GetConstInst(static_cast<uint32_t>(fieldIdx));

			// And now we can produce an OpAccessChain instruction.
			SpvInstruction opAccessChainInst = OpAccessChain(typePtrDeclInst, intBlockVarDeclInst, fieldIdxConstInst);
			return opAccessChainInst;
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
		void GlslToSpvGenerator::PrintFirstWords(std::vector<uint32_t>& storage) const {
			uint32_t magicNumber = 0x07230203;
			storage.push_back(magicNumber);

			uint32_t version = 0;
			uint32_t majorNumber = 1;
			uint32_t minorNumber = 3;
			version |= minorNumber << 8;
			version |= majorNumber << 16;
			storage.push_back(version);

			uint32_t generataorMagicNumber = 0;
			storage.push_back(generataorMagicNumber);

			uint32_t bound = GetLastGeneratedSpvId() + 1;
			storage.push_back(bound);

			uint32_t instructionSchemaReserved = 0;
			storage.push_back(instructionSchemaReserved);
		}
		void GlslToSpvGenerator::PrintInstructions(std::vector<uint32_t>& storage, const std::vector<SpvInstruction>& instructions) const {
			for (const SpvInstruction& spvInstruction : instructions) {
				PrintInstruction(storage, spvInstruction);
			}
		}
		void GlslToSpvGenerator::PrintInstruction(std::vector<uint32_t>& storage, const SpvInstruction& instruction) const {
			// 1. First word:
			//    - low-order 16 bits are the opcode enumerant
			//    - high-order 16 bits are the word count
			uint32_t firstWord = 0;
			firstWord |= static_cast<uint32_t>(instruction.GetOpCode());
			firstWord |= instruction.wordCount << 16;
			storage.push_back(firstWord);
			// 2. The second word, which is an optional Result type.
			if (instruction.HasResultType()) {
				storage.push_back(instruction.GetResultType());
			}
			// 3. The third word, an optional Result id.
			if (instruction.HasResultId()) {
				storage.push_back(instruction.GetResultId());
			}
			// 4. And finally all the other operands.
			for (const SpvInstOperand& operand : instruction.GetOperands()) {
				storage.push_back(operand.value);
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
			shaderProgram.SetVertexInputLayout(GenerateVertexInputLayoutDesc(vertexInputLayoutBlock));
			this->vertexInputLayoutBlock = vertexInputLayoutBlock;

			//spvEnv.vertexInputVarDecls = CreateVertexAttribDecls(shaderProgram.GetVertexInputLayout());
			//for (const std::shared_ptr<VarDecl>& vertexAttribVarDecl : spvEnv.vertexInputVarDecls) {
			//	spvEnv.AddVarDecl(vertexAttribVarDecl.get());
			//}
		}
		void GlslToSpvGenerator::VisitColorAttachmentsBlock(glsl::ColorAttachmentsBlock* colorAttachmentsBlock) {
			shaderProgram.SetColorAttachments(GenerateColorAttachments(colorAttachmentsBlock));
			this->colorAttachmentsBlock = colorAttachmentsBlock;

			//spvEnv.colorAttachmentVarDecls = CreateColorAttachmentVarDecls(shaderProgram.GetColorAttachments());
			//for (const std::shared_ptr<VarDecl>& colorAttachmentVarDecl : spvEnv.colorAttachmentVarDecls) {
			//	spvEnv.AddVarDecl(colorAttachmentVarDecl.get());
			//}
		}
		void GlslToSpvGenerator::VisitShaderBlock(glsl::ShaderBlock* shaderBlock) {
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
					ClearState();
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
					ClearState();
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
			// nameMangler << "var_" << intBlockDecl->GetName().lexeme;
			nameMangler << intBlockDecl->GetName().lexeme;
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

			spvEnv.AddIntBlockDecl(intBlockDecl);
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
			if (retType.specifier.type.tokenType == glsl::TokenType::VOID) {
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

			spvEnv.AddVarDecl(varDecl);
			std::string varName = std::string{identifier.lexeme};
			spvEnv.varDecls.insert({varName, varDeclInst});
		}

		void GlslToSpvGenerator::VisitBlockStmt(glsl::BlockStmt* blockStmt) {
			// Every block starts with a label.
			SpvInstruction labelInst = OpLabel();
			instructions.push_back(labelInst);
			for (const std::shared_ptr<Stmt>& stmt : blockStmt->GetStatements()) {
				stmt.get()->Accept(this);
			}
		}
		void GlslToSpvGenerator::VisitDeclStmt(glsl::DeclStmt* declStmt) {
			// TODO
		}
		void GlslToSpvGenerator::VisitExprStmt(glsl::ExprStmt* exprStmt) {
			exprStmt->GetExpression()->Accept(this);
		}

		void GlslToSpvGenerator::VisitInitListExpr(glsl::InitListExpr* initListExpr) {
			// TODO
		}
		void GlslToSpvGenerator::VisitAssignExpr(glsl::AssignExpr* assignExpr) {
			Expr* rvalue = assignExpr->GetRvalue();
			rvalue->Accept(this);
			SpvInstruction rvalueRes = this->result;
			// The lvalue should be handled based on what it actually is.
			Expr* lvalue = assignExpr->GetLvalue();
			VarExpr* lvalueVarExpr = dynamic_cast<VarExpr*>(lvalue);
			// 1. If the lvalue is a simple VarExpr, then there's no need for any kind of AccessChain instructions and
			//    we don't have to load the variable first, since what we want to do here is store the result, not use it.
			//    However, if the variable is part of an interface block, an OpAccessChain instruction must be used.
			//    Also, what if this assignment expression is part of a bigger expression? Something like "a = b = c"?
			//    Consider both cases: when the lvalue is a simple variable and when it's part of an interface block!
			if (lvalueVarExpr) {
				const Token& var = lvalueVarExpr->GetVariable();
				if (spvEnv.HasIntBlockVarDecl("gl_PerVertex", var.lexeme)) {
					SpvInstruction opAccessChainInst = AccessIntBlockField("gl_PerVertex", var.lexeme);
					instructions.push_back(opAccessChainInst);
					SpvInstruction lvalueStoreInst = OpStore(opAccessChainInst, rvalueRes);
					instructions.push_back(lvalueStoreInst);
				} else if (spvEnv.HasVarDecl(var.lexeme)) {
					std::string varName = std::string{var.lexeme};
					SpvInstruction varDeclInst = spvEnv.varDecls.find(varName)->second;
					SpvInstruction lvalueStoreInst = OpStore(varDeclInst, rvalueRes);
					instructions.push_back(lvalueStoreInst);
				}
			}
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
			const TypeSpec& typeSpec = ctorCallExpr->GetType();
			SpvInstruction typeDeclInst = GetTypeDeclInst(typeSpec);

			const std::vector<std::shared_ptr<Expr>>& ctorExprArgs = ctorCallExpr->GetArgs();
			std::vector<SpvInstruction> ctorInstArgs(ctorExprArgs.size());
			for (size_t i = 0; i < ctorInstArgs.size(); i++) {
				ctorExprArgs[i]->Accept(this);
				ctorInstArgs[i] = this->result;
			}

			SpvInstruction opConstantComposite = OpConstantComposite(typeDeclInst, ctorInstArgs);
			instructions.push_back(opConstantComposite);
			this->result = opConstantComposite;
		}
		void GlslToSpvGenerator::VisitVarExpr(glsl::VarExpr* varExpr) {
			const Token& var = varExpr->GetVariable();
			if (spvEnv.HasIntBlockVarDecl("gl_PerVertex", var.lexeme)) {
				SpvInstruction opAccessChainInst = AccessIntBlockField("gl_PerVertex", var.lexeme);
				instructions.push_back(opAccessChainInst);
				this->result = opAccessChainInst;
			} else if (spvEnv.HasVarDecl(var.lexeme)) {
				// Produce an OpLoad instruction. To do that we need:
				// 1. First, we need to know the id of the variable's type.
				VarDecl* varDecl = spvEnv.GetVarDecl(var.lexeme);
				std::string typeName = MangleTypeName(varDecl->GetVarType().specifier);
				SpvInstruction typeDeclInst = spvEnv.typeDecls.find(typeName)->second; // Assume that it already exists there!
				// 2. Second, we need the identifier of the OpVariable instruction.
				std::string varName = std::string{varDecl->GetVarName().lexeme};
				SpvInstruction varDeclInst = spvEnv.varDecls.find(varName)->second;
				
				// 3. Finally, we can now produce the appropriate OpLoad instruction.
				SpvInstruction opLoadInst = OpLoad(typeDeclInst, varDeclInst);
				instructions.push_back(opLoadInst);
				this->result = opLoadInst;
			}
		}
		void GlslToSpvGenerator::VisitIntConstExpr(glsl::IntConstExpr* intConstExpr) {
			// TODO: use ConstId to extract the value of the constant from the constant table.
			// The constant table should come from the parser. Seeing how we're already going to
			// reuse the parser's environment, adding a reference/pointer to the constant table
			// shouldn't be a lot of work.
			int constVal = intConstExpr->GetValue();
			this->result = GetConstInst(constVal);
		}
		void GlslToSpvGenerator::VisitUintConstExpr(glsl::UintConstExpr* uintConstExpr) {
			// TODO: use ConstId to extract the value of the constant from the constant table.
			// The constant table should come from the parser. Seeing how we're already going to
			// reuse the parser's environment, adding a reference/pointer to the constant table
			// shouldn't be a lot of work.
			unsigned int constVal = uintConstExpr->GetValue();
			this->result = GetConstInst(constVal);
		}
		void GlslToSpvGenerator::VisitFloatConstExpr(glsl::FloatConstExpr* floatConstExpr) {
			// TODO: use ConstId to extract the value of the constant from the constant table.
			// The constant table should come from the parser. Seeing how we're already going to
			// reuse the parser's environment, adding a reference/pointer to the constant table
			// shouldn't be a lot of work.
			float constVal = floatConstExpr->GetValue();
			this->result = GetConstInst(constVal);
		}
		void GlslToSpvGenerator::VisitDoubleConstExpr(glsl::DoubleConstExpr* doubleConstExpr) {
			// TODO: use ConstId to extract the value of the constant from the constant table.
			// The constant table should come from the parser. Seeing how we're already going to
			// reuse the parser's environment, adding a reference/pointer to the constant table
			// shouldn't be a lot of work.
			double constVal = doubleConstExpr->GetValue();
			this->result = GetConstInst(constVal);
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