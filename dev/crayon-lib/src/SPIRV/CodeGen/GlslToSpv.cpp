#include "SPIRV/CodeGen/GlslToSpv.h"

#include <iomanip>
#include <sstream>

namespace crayon {
	namespace spirv {

		using namespace glsl;

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
			typeDeclInsts.push_back(opTypeVoid);
			// 2.2 OpTypeFloat
			SpvInstruction opTypeFloat = OpTypeFloat(32);
			typeDeclInsts.push_back(opTypeFloat);
			// 2.3 OpTypeVector
			// Declaring a vec4 type.
			SpvInstruction opTypeVector3 = OpTypeVector(opTypeFloat, 3);
			typeDeclInsts.push_back(opTypeVector3);
			SpvInstruction opTypeVector4 = OpTypeVector(opTypeFloat, 4);
			typeDeclInsts.push_back(opTypeVector4);
			// Creating a type pointer to the vec4 type with the uniform storage class.
			SpvInstruction opTypePointerVec4 = OpTypePointer(opTypeVector3, SpvStorageClass::UNIFORM);
			typeDeclInsts.push_back(opTypePointerVec4);
			// 3. Constant instructions.
			SpvInstruction opConstantFl_0_5 = OpConstant(opTypeFloat, 0.5f);
			constants.push_back(opConstantFl_0_5);
			SpvInstruction opConstantFl_1_0 = OpConstant(opTypeFloat, 1.0f);
			constants.push_back(opConstantFl_1_0);
			// 3.1 Composite constant (vec4)
			std::vector<SpvInstruction> constituents(4);
			constituents[0] = opConstantFl_0_5;
			constituents[1] = opConstantFl_0_5;
			constituents[2] = opConstantFl_0_5;
			constituents[3] = opConstantFl_1_0;
			SpvInstruction opConstantCompositeVec4 = OpConstantComposite(opTypeVector4, constituents);
			constants.push_back(opConstantCompositeVec4);
			// 4. Functions
			// Creating a function type of the form: "void fun_name()" where "fun_name" can be any function name.
			// Function has no parameters and doesn't return anything.
			SpvInstruction opTypeFunctionVoid = OpTypeFunction(opTypeVoid);
			typeDeclInsts.push_back(opTypeFunctionVoid);
			// Creating a void function with no parameters.
			SpvInstruction opVoidFunction = OpFunction(opTypeFunctionVoid, SpvFunctionControl::NONE);
			instructions.push_back(opVoidFunction);
			SpvInstruction voidFunBlockStart = OpLabel();
			instructions.push_back(voidFunBlockStart);
			// 5. Variable declaration.
			SpvInstruction vec3InTypePointer = OpTypePointer(opTypeVector3, SpvStorageClass::INPUT);
			typeDeclInsts.push_back(vec3InTypePointer);
			SpvInstruction vec4OutTypePointer = OpTypePointer(opTypeVector4, SpvStorageClass::OUTPUT);
			typeDeclInsts.push_back(vec4OutTypePointer);
			// a) Variable without an initializer.
			SpvInstruction vec3InVarDecl = OpVariable(vec3InTypePointer, SpvStorageClass::INPUT);
			SpvInstruction vec4OutVarDecl = OpVariable(vec4OutTypePointer, SpvStorageClass::OUTPUT);
			// b) Variable with an initializer.
			// SpvInstruction vec4VarDecl = OpVariable(vec4TypePointer, SpvStorageClass::OUTPUT, opConstantCompositeVec4);
			inVars.push_back(vec3InVarDecl);
			outVars.push_back(vec4OutVarDecl);
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
			std::vector<SpvInstruction> interfaceVars(inVars.size() + outVars.size());
			size_t i = 0;
			for (; i < inVars.size(); i++) {
				interfaceVars[i] = inVars[i];
			}
			for (int j = 0; j < outVars.size(); i++, j++) {
				interfaceVars[i] = outVars[j];
			}
			this->entryPointInst = OpEntryPoint(SpvExecutionModel::FRAGMENT, opVoidFunction,
				                                "main", interfaceVars);
		}

		void GlslToSpvGenerator::ClearState() {
			spvEnv.Clear();

			decorations.clear();
			typeDeclInsts.clear();
			typePtrInsts.clear();
			constants.clear();
			globalVars.clear();

			inVars.clear();
			outVars.clear();

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
			PrintTypeInstructions(spvAsmText);
			PrintTypePtrInstructions(spvAsmText);
			// PrintConstantInstructions(spvAsmText);

			PrintGlobalVariables(spvAsmText);

			PrintFunctionInstructions(spvAsmText);

			return spvAsmText.str();
		}

		SpvInstruction GlslToSpvGenerator::GetTypePointerInstruction(GlslBasicType glslType, SpvStorageClass storageClass) {
			std::string typePtrMangledName = MangleTypePointerName(glslType, storageClass);
			auto searchRes = spvEnv.typePtrs.find(typePtrMangledName);
			if (searchRes == spvEnv.typePtrs.end()) {
				// Create a type pointer of the requested "format".
				SpvInstruction typeDeclInst = GetTypeDeclarationInstruction(glslType);
				SpvInstruction typePtrInst = OpTypePointer(typeDeclInst, storageClass);
				spvEnv.typePtrs.insert({typePtrMangledName, typePtrInst});
				typePtrInsts.push_back(typePtrInst);
				return typePtrInst;
			}
			return searchRes->second;
		}
		SpvInstruction GlslToSpvGenerator::GetTypeDeclarationInstruction(glsl::GlslBasicType glslType) {
			std::string typeMangledName = MangleTypeName(glslType);
			auto searchRes = spvEnv.typeDecls.find(typeMangledName);
			if (searchRes == spvEnv.typeDecls.end()) {
				// Create the requested type.
				SpvInstruction typeDeclInst = CreateTypeDeclarationInstruction(glslType);
				spvEnv.typeDecls.insert({typeMangledName, typeDeclInst});
				typeDeclInsts.push_back(typeDeclInst);
				return typeDeclInst;
			}
			return searchRes->second;
		}
		SpvInstruction GlslToSpvGenerator::GetTypeDeclarationInstruction(const glsl::VarDecl* varDecl) {
			const FullSpecType& varType = varDecl->GetVarType();
			const Token& identifier = varDecl->GetVarName();
			
			// Handle arrays too.

			return SpvInstruction();
		}
		SpvInstruction GlslToSpvGenerator::GetTypeFunDeclInst(const glsl::FunProto* funProto) {
			std::string funMangledName = MangleTypeFunctionName(funProto);
			auto searchRes = spvEnv.typeDecls.find(funMangledName);
			if (searchRes == spvEnv.typeDecls.end()) {
				// Create the requested type.
				SpvInstruction typeFunDeclInst = CreateTypeFunDeclInst(funProto);
				spvEnv.typeDecls.insert({funMangledName, typeFunDeclInst});
				typeDeclInsts.push_back(typeFunDeclInst);
				return typeFunDeclInst;
			}
			return searchRes->second;
		}

		SpvInstruction GlslToSpvGenerator::CreateTypeDeclarationInstruction(glsl::GlslBasicType glslType) {
			if (glslType == glsl::GlslBasicType::VOID)
				return OpTypeVoid();

			if (IsScalarType(glslType)) {
				return CreateScalarTypeDeclarationInstruction(glslType);
			} else if (IsVectorType(glslType)) {
				return CreateVectorTypeDeclarationInstruction(glslType);
			} else if (IsMatrixType(glslType)) {
				assert(false && "Matrices aren't implemented yet!");
				return CreateMatrixTypeDeclarationInstruction(glslType);
			} else {
				assert(false && "Arrays aren't implemented yet!");
			}
			return SpvInstruction();
		}
		SpvInstruction GlslToSpvGenerator::CreateScalarTypeDeclarationInstruction(glsl::GlslBasicType glslType) {
			switch (glslType) {
				case GlslBasicType::BOOL: {
					return OpTypeBool();
				}
				case GlslBasicType::INT: {
					return OpTypeInt(32, SpvSignedness::SIGNED);
				}
				case GlslBasicType::UINT: {
					return OpTypeInt(32, SpvSignedness::UNSIGNED);
				}
				case GlslBasicType::FLOAT: {
					return OpTypeFloat(32);
				}
				case GlslBasicType::DOUBLE: {
					return OpTypeFloat(64);
				}
				default: {
					assert(false && "Non-scalar type provided!");
					return SpvInstruction();
				}
			}
		}
		SpvInstruction GlslToSpvGenerator::CreateVectorTypeDeclarationInstruction(glsl::GlslBasicType glslType) {
			GlslBasicType fundamentalType = GetFundamentalType(glslType);
			SpvInstruction fundTypeDeclInst = GetTypeDeclarationInstruction(fundamentalType);
			uint32_t componentCount = static_cast<uint32_t>(GetColVecNumberOfRows(glslType));
			return OpTypeVector(fundTypeDeclInst, componentCount);
		}
		SpvInstruction GlslToSpvGenerator::CreateMatrixTypeDeclarationInstruction(glsl::GlslBasicType glslType) {
			// TODO
			return SpvInstruction();
		}
		SpvInstruction GlslToSpvGenerator::CreateTypeFunDeclInst(const glsl::FunProto* funProto) {
			const FullSpecType& retType = funProto->GetReturnType();
			glsl::GlslBasicType glslRetType = TokenTypeToGlslBasicType(retType.specifier.type.tokenType);
			SpvInstruction retTypeDeclInst = GetTypeDeclarationInstruction(glslRetType);
			return OpTypeFunction(retTypeDeclInst);
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
			size_t strWordLen = ops.size() - 2 - inVars.size() - outVars.size(); // 2 comes from the first two operands.
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
			PrintInstructions(out, typeDeclInsts);
		}
		void GlslToSpvGenerator::PrintTypePtrInstructions(std::ostream& out) const {
			PrintInstructions(out, typePtrInsts);
		}
		void GlslToSpvGenerator::PrintConstantInstructions(std::ostream& out) const {
			PrintInstructions(out, constants);
		}
		void GlslToSpvGenerator::PrintGlobalVariables(std::ostream& out) const {
			PrintInstructions(out, inVars);
			PrintInstructions(out, outVars);
			PrintInstructions(out, globalVars);
		}
		void GlslToSpvGenerator::PrintFunctionInstructions(std::ostream& out) const {
			PrintInstructions(out, instructions);
		}

		void GlslToSpvGenerator::PrintInstructions(std::ostream& out, const std::vector<SpvInstruction>& instructions) const {
			for (const SpvInstruction& spvInstruction : instructions) {
				if (spvInstruction.HasResultId()) {
					out << std::setw(idFieldWidth);
				}
				else {
					size_t spvInstructionShift = idFieldWidth + 3; // 3 is from the " = " sequence of characters
					std::fill_n(
						std::ostream_iterator<char>(out),
						spvInstructionShift, ' ');
				}
				PrintSpvInstruction(out, spvInstruction);
				out << "\n";
			}
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
			for (const VertexAttribDesc& vertexAttribDesc : vertexInputLayoutDesc.attributes) {
				int location = GetVertexAttribChannelNum(vertexAttribDesc.channel);
				GlslBasicType glslBasicType = vertexAttribDesc.glslType;
				SpvInstruction typePtrInst = GetTypePointerInstruction(glslBasicType, SpvStorageClass::INPUT);
				SpvInstruction varDeclInst = OpVariable(typePtrInst, SpvStorageClass::INPUT);
				inVars.push_back(varDeclInst);
				// globalVars.push_back(varDeclInst);
				SpvInstruction locDecInst = OpDecorateLocation(varDeclInst, static_cast<uint32_t>(location));
				decorations.push_back(locDecInst);
			}
		}
		void GlslToSpvGenerator::VisitColorAttachmentsBlock(glsl::ColorAttachmentsBlock* colorAttachmentsBlock) {
			// TODO
		}
		void GlslToSpvGenerator::VisitShaderBlock(glsl::ShaderBlock* shaderBlock) {
			// ClearState();
			ShaderType shaderType = shaderBlock->GetShaderType();
			switch (shaderType) {
				case ShaderType::VS: {
					std::shared_ptr<InterfaceBlockDecl> glPerVertex = CreatePerVertexIntBlockDecl();
					glPerVertex->Accept(this);

					spvEnv.execModel = SpvExecutionModel::VERTEX;
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
			// 1. First we handle the block's member declarations. (Types, Type Pointers, Decorations, etc.)
			std::string_view intBlockName = intBlockDecl->GetName().lexeme;
			std::vector<SpvInstruction> intBlockMembers(intBlockDecl->GetFieldCount());
			for (const std::shared_ptr<glsl::VarDecl>& field : intBlockDecl->GetFields()) {
				// Every field must have a type, but not necessarily a type pointer.
				// A type pointer is only needed if we access a block member. This, however,
				// is going to be handled at the time of access.

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
				// Don't forget about the shader interface variables (the "in" and "out" ones).
				std::vector<SpvInstruction> interfaceVars(inVars.size() + outVars.size());
				size_t i = 0;
				for (; i < inVars.size(); i++) {
					interfaceVars[i] = inVars[i];
				}
				for (int j = 0; j < outVars.size(); i++, j++) {
					interfaceVars[i] = outVars[j];
				}
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

			GlslBasicType glslBasicType = TokenTypeToGlslBasicType(varType.specifier.type.tokenType);
			SpvInstruction typePtrInst = GetTypePointerInstruction(glslBasicType, storageClass);

			// Consider the IsArray() method 
			//if (fullSpecType.specifier.IsArray()) {
			//    // TODO
			//}

			SpvInstruction varDeclInst = OpVariable(typePtrInst, storageClass);
			if (storageClass == SpvStorageClass::INPUT)
				inVars.push_back(varDeclInst);
			else if (storageClass == SpvStorageClass::OUTPUT)
				outVars.push_back(varDeclInst);
			else
				globalVars.push_back(varDeclInst);

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

		std::string MangleTypeName(const glsl::TypeSpec& typeSpec) {
			return std::string(typeSpec.type.lexeme);
		}
		std::string MangleTypeName(glsl::GlslBasicType glslType) {
			return std::string(GetGlslBasicTypeName(glslType));
		}
		std::string MangleTypePointerName(const glsl::TypeSpec& typeSpec, SpvStorageClass storageClass) {
			std::stringstream nameMangler;
			nameMangler << "type_ptr_";
			nameMangler << SpvStorageClassToString(storageClass) << "_";
			nameMangler << typeSpec.type.lexeme;
			return nameMangler.str();
		}
		std::string MangleTypePointerName(glsl::GlslBasicType glslType, SpvStorageClass storageClass) {
			std::stringstream nameMangler;
			nameMangler << "type_ptr_";
			nameMangler << SpvStorageClassToString(storageClass) << "_";
			nameMangler << GetGlslBasicTypeName(glslType);
			return nameMangler.str();
		}

		std::string MangleTypeFunctionName(const glsl::FunProto* funProto) {
			std::stringstream nameMangler;
			// 1. Return type.
			const FullSpecType& retType = funProto->GetReturnType();
			glsl::GlslBasicType glslRetType = TokenTypeToGlslBasicType(retType.specifier.type.tokenType);
			nameMangler << GetGlslBasicTypeName(glslRetType);
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
			SpvStorageClass storageClass = SpvStorageClass::INPUT;
			return MangleTypePointerName(typeSpec, storageClass);
		}
		std::string MangleTypePointerName(glsl::MatPropDecl* matProp) {
			// TODO
			return std::string();
		}
		std::string MangleTypePointerName(glsl::ColorAttachmentDecl* colorAttachment) {
			const TypeSpec& typeSpec = colorAttachment->GetTypeSpec();
			SpvStorageClass storageClass = SpvStorageClass::INPUT;
			return MangleTypePointerName(typeSpec, storageClass);
		}

	}
}