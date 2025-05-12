#include "GLSL/CodeGen/GlslWriter.h"

namespace crayon {
	namespace glsl {

		GlslWriter::GlslWriter(const GlslWriterConfig& config)
			: config(config) {
		}

		// Decl visit methods
		void GlslWriter::VisitTransUnit(TransUnit* transUnit) {
			for (const std::shared_ptr<Decl>& decl : transUnit->GetDeclarations()) {
				decl->Accept(this);
				src << "\n";
			}
		}
		void GlslWriter::VisitFunDecl(FunDecl* funDecl) {
			const FunProto& funProto = funDecl->GetFunctionPrototype();
			WriteFunctionPrototype(funProto);

			if (funDecl->IsFunDecl()) {
				src << ";";
				return;
			}

			std::shared_ptr<BlockStmt> funStmts = funDecl->GetBlockStatement();
			VisitBlockStmt(funStmts.get());
		}
		void GlslWriter::VisitQualDecl(QualDecl* qualDecl) {
			const TypeQual& typeQual = qualDecl->GetTypeQualifier();
			WriteTypeQualifier(typeQual);
			src << ";";
		}
		void GlslWriter::VisitVarDecl(VarDecl* varDecl) {
			const FullSpecType& varType = varDecl->GetVariableType();
			const Token& identifier = varDecl->GetVariableName();

			WriteFullySpecifiedType(varType);
			src << " " << identifier.lexeme;
			src << ";";
		}

		// Stmt visit methods
		void GlslWriter::VisitBlockStmt(BlockStmt* blockStmt) {
			WriteOpeningBlockBrace();
			// [TODO]
			// WriteClosingBlockBrace();
			src << "}";
		}

		// Helper methods
		std::string GlslWriter::GetSrcCodeStr() const {
			return src.str();
		}

		void GlslWriter::WriteFullySpecifiedType(const FullSpecType& fullSpecType) {
			if (!fullSpecType.qualifier.Empty()) {
				WriteTypeQualifier(fullSpecType.qualifier);
				src << " ";
			}
			src << fullSpecType.specifier.type.lexeme;
		}
		void GlslWriter::WriteTypeQualifier(const TypeQual& typeQual) {
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
			src.seekp(-1, src.cur);
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
				src.seekp(-2, src.cur); // to remove the last two characters: ", "
			src << ")";
		}

		void GlslWriter::WriteFunctionPrototype(const FunProto& funProto)
		{
			const FullSpecType& retType = funProto.GetReturnType();
			const Token& funName = funProto.GetFunctionName();

			WriteFullySpecifiedType(retType);
			src << " " << funName.lexeme;
			src << "(";
			if (!funProto.FunctionParameterListEmpty()) {
				WriteFunctionParameterList(funProto.GetFunctionParameterList());
			}
			src << ")";
		}
		void GlslWriter::WriteFunctionParameterList(const FunParamList& funParamList)
		{
			const std::list<FunParam>& funParams = funParamList.GetFunctionParameters();
			for (const FunParam& funParam : funParams) {
				const FullSpecType& paramType = funParam.GetVariableType();
				WriteFullySpecifiedType(paramType);
				src << " ";
				if (funParam.HasName()) {
					const Token& identifier = funParam.GetVariableName();
					src << identifier.lexeme;
				}
				src << ", ";
			}
			if (!funParams.empty())
				src.seekp(-2, src.cur); // to remove the last two characters: ", "
		}

		void GlslWriter::WriteOpeningBlockBrace() {
			if (config.leftBraceOnSameLine)
				src << " {";
			else
				src << "\n{";
			src << "\n";
		}

		// Expression visit methods
		void GlslWriter::VisitBinaryExpr(Binary* binaryExpr) {
			// [TODO]
		}
		void GlslWriter::VisitIntConstExpr(IntConst* intConstExpr) {
			// [TODO]
		}
		void GlslWriter::VisitGroupExpr(GroupExpr* groupExpr) {
			// [TODO]
		}
	}
}