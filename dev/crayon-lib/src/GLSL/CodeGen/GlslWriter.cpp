#include "GLSL/CodeGen/GlslWriter.h"

#include <algorithm>
#include <iterator>

namespace crayon {
	namespace glsl {

		GlslWriter::GlslWriter(const GlslWriterConfig& config)
			: config(config) {
		}

		void GlslWriter::ResetInternalState() {
			indentLvl = 0;
			src.str("");
			src.clear();
		}
		void GlslWriter::PrintNewLine() {
			src << "\n";
		}

		void GlslWriter::VisitShaderProgramBlock(ShaderProgramBlock* programBlock) {
			std::string_view name = programBlock->GetShaderProgramName();
			src << "ShaderProgram \"" << name << "\"";
			WriteOpeningBlockBrace();
			src << "\n";
			indentLvl++;
			for (const std::shared_ptr<Block>& block : programBlock->GetBlocks()) {
				WriteIndentation();
				block->Accept(this);
				src << "\n";
			}
			indentLvl--;
			src << "}";
		}
		void GlslWriter::VisitFixedStagesConfigBlock(FixedStagesConfigBlock* fixedStagesConfigBlock) {
			// TODO
		}
		void GlslWriter::VisitMaterialPropertiesBlock(MaterialPropertiesBlock* materialPropertiesBlock) {
			std::string_view matPropsBlockName = ExtractStringLiteral(materialPropertiesBlock->GetName());
			src << "MaterialProperties \"" << matPropsBlockName << "\"";
			WriteOpeningBlockBrace();
			src << "\n";
			indentLvl++;
			for (const std::shared_ptr<MatPropDecl>& matPropDecl : materialPropertiesBlock->GetMatPropDecls()) {
				WriteIndentation();
				const Token& type = matPropDecl->GetType();
				const Token& name = matPropDecl->GetName();
				src << type.lexeme;
				src << " ";
				src << name.lexeme;
				src << ";\n";
			}
			// RemoveFromOutput(1);
			indentLvl--;
			WriteClosingBlockBrace();
		}
		void GlslWriter::VisitVertexInputLayoutBlock(VertexInputLayoutBlock* vertexInputLayoutBlock) {
			src << "VertexInputLayout ";
			WriteOpeningBlockBrace();
			src << "\n";
			indentLvl++;
			for (const std::shared_ptr<VertexAttribDecl>& vertexAttribDecl : vertexInputLayoutBlock->GetAttribDecls()) {
				WriteIndentation();
				const TypeSpec& typeSpec = vertexAttribDecl->GetTypeSpec();
				src << typeSpec.type.lexeme;
				src << " ";
				src << vertexAttribDecl->GetName().lexeme;
				src << " : ";
				src << vertexAttribDecl->GetChannel().lexeme;
				src << ";\n";
			}
			// RemoveFromOutput(1);
			indentLvl--;
			WriteClosingBlockBrace();
		}
		void GlslWriter::VisitColorAttachmentsBlock(ColorAttachmentsBlock* colorAttachmentsBlock) {
			src << "ColorAttachments ";
			WriteOpeningBlockBrace();
			src << "\n";
			indentLvl++;
			for (const std::shared_ptr<ColorAttachmentDecl>& colorAttachment : colorAttachmentsBlock->GetColorAttachments()) {
				WriteIndentation();
				const TypeSpec& typeSpec = colorAttachment->GetTypeSpec();
				src << typeSpec.type.lexeme;
				src << " ";
				src << colorAttachment->GetName().lexeme;
				src << " : ";
				src << colorAttachment->GetChannel().lexeme;
				src << ";\n";
			}
			// RemoveFromOutput(1);
			indentLvl--;
			WriteClosingBlockBrace();
		}
		void GlslWriter::VisitShaderBlock(ShaderBlock* shaderBlock) {
			switch (shaderBlock->GetShaderType()) {
				case ShaderType::VS:
					src << "VertexShader ";
					break;
				case ShaderType::TCS:
					src << "TessellationControlShader ";
					break;
				case ShaderType::TES:
					src << "TessellationEvaluationShader ";
					break;
				case ShaderType::GS:
					src << "GeometryShader ";
					break;
				case ShaderType::FS:
					src << "FragmentShader ";
					break;
			}
			WriteOpeningBlockBrace();
			src << "\n";
			indentLvl++;
			WriteIndentation();
			src << "BEGIN\n";
			shaderBlock->GetTranslationUnit()->Accept(this);
			WriteIndentation();
			src << "END\n";
			indentLvl--;
			WriteClosingBlockBrace();
		}

		// Decl visit methods
		void GlslWriter::VisitTransUnit(TransUnit* transUnit) {
			// ResetInternalState();
			for (const std::shared_ptr<Decl>& decl : transUnit->GetDeclarations()) {
				WriteIndentation();
				decl->Accept(this);
				src << "\n";
			}
			// Do we want to leave the last new line character?
		}
		void GlslWriter::VisitStructDecl(StructDecl* structDecl) {
			WriteStructDecl(structDecl);
			src << ";";
		}
		void GlslWriter::VisitInterfaceBlockDecl(InterfaceBlockDecl* intBlockDecl) {
			WriteTypeQualifier(intBlockDecl->GetTypeQualifier());
			const Token& name = intBlockDecl->GetName();
			src << " " << name.lexeme << " ";
			WriteOpeningBlockBrace();
			src << "\n";
			indentLvl++;
			for (const std::shared_ptr<VarDecl>& varDecl : intBlockDecl->GetFields()) {
				WriteIndentation();
				varDecl->Accept(this);
				src << "\n";
			}
			indentLvl--;
			WriteClosingBlockBrace();
			src << ";";
		}
		void GlslWriter::VisitDeclList(DeclList* declList) {
			const FullSpecType& fullSpecType = declList->GetFullSpecType();
			WriteFullySpecifiedType(fullSpecType);
			for (const auto& varDecl : declList->GetDecls()) {
				const Token& varName = varDecl->GetVarName();
				src << " " << varName.lexeme;
				if (varDecl->IsVarArray()) {
					WriteArrayDimensions(varDecl->GetDimensions());
				}
				if (varDecl->HasInitializerExpr()) {
					src << " = ";
					varDecl->GetInitializerExpr()->Accept(this);
				}
				src << ",";
			}
			RemoveFromOutput(1);
			src << ";";
		}
		void GlslWriter::VisitFunDecl(FunDecl* funDecl) {
			std::shared_ptr<FunProto> funProto = funDecl->GetFunProto();
			WriteFunctionPrototype(funProto);
			if (funDecl->IsFunDecl()) {
				src << ";";
				return;
			} else {
				src << " "; // before the opening bracket
			}
			std::shared_ptr<BlockStmt> funStmts = funDecl->GetBlockStmt();
			VisitBlockStmt(funStmts.get());
		}
		void GlslWriter::VisitQualDecl(QualDecl* qualDecl) {
			const TypeQual& typeQual = qualDecl->GetTypeQualifier();
			WriteTypeQualifier(typeQual);
			src << ";";
		}
		void GlslWriter::VisitVarDecl(VarDecl* varDecl) {
			const FullSpecType& varType = varDecl->GetVarType();
			const Token& identifier = varDecl->GetVarName();

			WriteFullySpecifiedType(varType);
			src << " " << identifier.lexeme;
			if (varDecl->IsVarArray()) {
				WriteArrayDimensions(varDecl->GetDimensions());
			}
			if (varDecl->HasInitializerExpr()) {
				src << " = ";
				varDecl->GetInitializerExpr()->Accept(this);
			}
			src << ";";
		}

		// Stmt visit methods
		void GlslWriter::VisitBlockStmt(BlockStmt* blockStmt) {
			WriteOpeningBlockBrace();
			src << "\n";
			indentLvl++;
			for (const std::shared_ptr<Stmt>& stmt : blockStmt->GetStatements()) {
				stmt.get()->Accept(this);
				src << "\n";
			}
			indentLvl--;
			WriteClosingBlockBrace();
		}
		void GlslWriter::VisitDeclStmt(DeclStmt* declStmt) {
			WriteIndentation();
			declStmt->GetDeclaration()->Accept(this);
			// src << ";";
		}
		void GlslWriter::VisitExprStmt(ExprStmt* exprStmt) {
			WriteIndentation();
			exprStmt->GetExpression()->Accept(this);
			src << ";";
		}

		// Expression visit methods
		void GlslWriter::VisitInitListExpr(InitListExpr* initListExpr) {
			indentLvl++;
			initListLvl++;

			/*
			// WriteOpeningBlockBrace();
			src << "{";
			if (initListLvl == 1) {
				src << "\n";
			}
			for (const std::shared_ptr<Expr>& initExpr : InitListExpr->GetInitExprs()) {
				if (initListLvl == 1) {
					WriteIndentation();
				}
				initExpr.get()->Accept(this);
				src << ", ";
				if (initListLvl == 1) {
					RemoveFromOutput(1);
					src << "\n";
				}
			}
			// RemoveFromOutput(2);
			if (initListLvl == 1) {
				RemoveFromOutput(1);
				src << "\n";
			} else {
				RemoveFromOutput(2);
			}
			src << "}";
			*/
			if (initListLvl == 1) {
				WriteInitListFirst(initListExpr);
			} else {
				WriteInitListRest(initListExpr);
			}

			initListLvl--;
			indentLvl--;
		}
		void GlslWriter::VisitAssignExpr(AssignExpr* assignExpr) {
			Expr* lvalue = assignExpr->GetLvalue();
			Expr* rvalue = assignExpr->GetRvalue();

			lvalue->Accept(this);
			src << " " << assignExpr->GetAssignOp().lexeme << " ";
			rvalue->Accept(this);
		}
		void GlslWriter::VisitBinaryExpr(BinaryExpr* binaryExpr) {
			Expr* left = binaryExpr->GetLeftExpr();
			Expr* right = binaryExpr->GetRightExpr();
			const Token& op = binaryExpr->GetOperator();

			left->Accept(this);
			src << " " << op.lexeme << " ";
			right->Accept(this);
		}
		void GlslWriter::VisitUnaryExpr(UnaryExpr* unaryExpr) {
			Expr* expr = unaryExpr->GetExpr();
			const Token& op = unaryExpr->GetOperator();

			src << op.lexeme;
			expr->Accept(this);
		}
		void GlslWriter::VisitFieldSelectExpr(FieldSelectExpr* fieldSelectExpr) {
			Expr* target = fieldSelectExpr->GetTarget();
			const Token& field = fieldSelectExpr->GetField();

			target->Accept(this);
			src << ".";
			src << field.lexeme;
		}
		void GlslWriter::VisitFunCallExpr(FunCallExpr* funCallExpr) {
			Expr* target = funCallExpr->GetTarget();
			target->Accept(this);

			if (!funCallExpr->ArgsEmpty()) {
				const FunCallArgList& args = funCallExpr->GetArgs();
				WriteFunctionCallArgList(args);
			}
		}
		void GlslWriter::VisitCtorCallExpr(CtorCallExpr* ctorCallExpr) {
			const Token& type = ctorCallExpr->GetType();
			src << type.lexeme;
			src << "(";
			if (!ctorCallExpr->ArgsEmpty()) {
				const FunCallArgList& args = ctorCallExpr->GetArgs();
				WriteFunctionCallArgList(args);
			}
			src << ")";
		}
		void GlslWriter::VisitVarExpr(VarExpr* varExpr) {
			const Token& var = varExpr->GetVariable();
			src << var.lexeme;
		}
		void GlslWriter::VisitIntConstExpr(IntConstExpr* intConstExpr) {
			const Token& intConst = intConstExpr->GetIntConst();
			src << intConst.lexeme;
		}
		void GlslWriter::VisitUintConstExpr(UintConstExpr* uintConstExpr) {
			const Token& uintConst = uintConstExpr->GetUintConst();
			src << uintConst.lexeme;
		}
		void GlslWriter::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			const Token& floatConst = floatConstExpr->GetFloatConst();
			src << floatConst.lexeme;
		}
		void GlslWriter::VisitDoubleConstExpr(DoubleConstExpr* doubleConstExpr) {
			const Token& doubleConst = doubleConstExpr->GetDoubleConst();
			src << doubleConst.lexeme;
		}
		void GlslWriter::VisitGroupExpr(GroupExpr* groupExpr) {
			src << "(";
			groupExpr->Accept(this);
			src << ")";
		}

		std::string GlslWriter::GetSrcCodeStr() const {
			return src.str();
		}

		// Helper methods
		void GlslWriter::WriteFullySpecifiedType(const FullSpecType& fullSpecType) {
			if (!fullSpecType.qualifier.Empty()) {
				WriteTypeQualifier(fullSpecType.qualifier);
				src << " ";
			}
			if (fullSpecType.specifier.typeDecl) {
				WriteStructDecl(fullSpecType.specifier.typeDecl.get());
			} else {
				src << fullSpecType.specifier.type.lexeme;
			}
			if (fullSpecType.specifier.IsArray()) {
				WriteArrayDimensions(fullSpecType.specifier.dimensions);
			}
		}
		void GlslWriter::WriteArrayDimensions(const std::vector<std::shared_ptr<Expr>>& dimensions) {
			for (const std::shared_ptr<Expr>& dimExpr : dimensions) {
				src << "[";
				if (dimExpr) {
					// Dimension size can be left unspecified.
					dimExpr.get()->Accept(this);
				}
				src << "]";
			}
		}
		void GlslWriter::WriteTypeQualifier(const TypeQual& typeQual) {
			if (typeQual.Empty())
				return;

			if (!typeQual.layout.empty()) {
				WriteLayoutQualifier(typeQual.layout);
				src << " ";
			}
			if (typeQual.storage.has_value()) {
				src << typeQual.storage.value().lexeme;
				src << " ";
			}
			if (typeQual.precise.has_value()) {
				src << typeQual.precise.value().lexeme;
				src << " ";
			}
			if (typeQual.precision.has_value()) {
				src << typeQual.precision.value().lexeme;
				src << " ";
			}
			
			// [TODO]: add more qualifiers

			// To remove a single ' ' after the last type qualifier.
			// We might now have a type specifier later on, in which case
			// the space will be undesired. So it's more appropriate to
			// handled this space in the caller.
			RemoveFromOutput(1);
		}
		void GlslWriter::WriteLayoutQualifier(const std::list<LayoutQualifier>& layoutQualifiers) {
			src << "layout (";
			for (const LayoutQualifier& qualifier : layoutQualifiers) {
				src << qualifier.name.lexeme;
				if (qualifier.value.has_value()) {
					src << " = " << qualifier.value.value();
				}
				src << ", ";
			}
			if (!layoutQualifiers.empty())
				RemoveFromOutput(2); // to remove the last two characters: ", "
			src << ")";
		}

		void GlslWriter::WriteStructDecl(StructDecl* structDecl) {
			// WriteIndentation();

			src << "struct ";
			const Token& name = structDecl->GetName();
			if (name.tokenType == TokenType::IDENTIFIER) {
				// If it's not an unnamed structure.
				src << name.lexeme << " ";
			}

			// src << "{\n";
			WriteOpeningBlockBrace();
			src << "\n";

			indentLvl++;
			for (const std::shared_ptr<VarDecl>& varDecl : structDecl->GetFields()) {
				WriteIndentation();
				varDecl->Accept(this);
				src << "\n";
			}
			indentLvl--;

			// src << "}";
			WriteClosingBlockBrace();
		}

		void GlslWriter::WriteInitListFirst(InitListExpr* initListExpr) {
			// WriteOpeningBlockBrace();
			src << "{\n";
			for (const std::shared_ptr<Expr>& initExpr : initListExpr->GetInitExprs()) {
				WriteIndentation();
				initExpr.get()->Accept(this);
				src << ",\n";
			}
			// 1 - if you want a trailing comma, and
			// 2 - if you don't.
			RemoveFromOutput(1);
			src << "\n}";
		}
		void GlslWriter::WriteInitListRest(InitListExpr* initListExpr) {
			// WriteOpeningBlockBrace();
			src << "{";
			for (const std::shared_ptr<Expr>& initExpr : initListExpr->GetInitExprs()) {
				initExpr.get()->Accept(this);
				src << ", ";
			}
			RemoveFromOutput(2);
			src << "}";
		}

		void GlslWriter::WriteFunctionPrototype(std::shared_ptr<FunProto> funProto) {
			const FullSpecType& retType = funProto->GetReturnType();
			const Token& funName = funProto->GetFunctionName();

			// WriteIndentation();

			WriteFullySpecifiedType(retType);
			src << " " << funName.lexeme;
			src << "(";
			if (!funProto->FunParamListEmpty()) {
				WriteFunctionParameterList(funProto->GetFunParamList());
			}
			src << ")";
		}
		void GlslWriter::WriteFunctionParameterList(const std::vector<std::shared_ptr<FunParam>>& funParamList) {
			for (const std::shared_ptr<FunParam>& funParam : funParamList) {
				const FullSpecType& paramType = funParam->GetVarType();
				WriteFullySpecifiedType(paramType);
				src << " ";
				if (funParam->HasName()) {
					const Token& identifier = funParam->GetVarName();
					src << identifier.lexeme;
				}
				src << ", ";
			}
			if (!funParamList.empty())
				RemoveFromOutput(2); // to remove the last two characters: ", "
		}
		void GlslWriter::WriteFunctionCallArgList(const FunCallArgList& funCallArgList) {
			const std::list<std::shared_ptr<Expr>>& args = funCallArgList.GetArgs();
			for (const std::shared_ptr<Expr>& arg : args) {
				arg->Accept(this);
				src << ", ";
			}
			if (!args.empty())
				RemoveFromOutput(2); // to remove the last two characters: ", "
		}

		void GlslWriter::WriteOpeningBlockBrace() {
			if (config.openingBraceOnSameLine) {
				src << "{";
			} else {
				src << "\n";
				WriteIndentation();
				src << "{";
			}
		}

		void GlslWriter::WriteClosingBlockBrace() {
			WriteIndentation();
			src << "}";
		}

		void GlslWriter::WriteIndentation() {
			std::fill_n(
				std::ostream_iterator<char>(src),
				config.indentCount * indentLvl,
				config.indentChar);
		}

		void GlslWriter::RemoveFromOutput(int count) {
			src.seekp(-count, src.cur);
		}
		
	}
}