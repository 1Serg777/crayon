#include "GLSL/CodeGen/GlslWriter.h"

#include <algorithm>
#include <iterator>

namespace crayon {
	namespace glsl {

		GlslWriter::GlslWriter(const GlslWriterConfig& config)
			: config(config) {
		}

		// Decl visit methods
		void GlslWriter::VisitTransUnit(TransUnit* transUnit) {
			ResetInternalState();
			for (const std::shared_ptr<Decl>& decl : transUnit->GetDeclarations()) {
				decl->Accept(this);
				src << "\n";
			}
			// Do we want to leave the last new line character?
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
			indentLvl++;
			WriteOpeningBlockBrace();
			for (const std::shared_ptr<Stmt>& stmt : blockStmt->GetStatements()) {
				stmt.get()->Accept(this);
				src << "\n";
			}
			// WriteClosingBlockBrace(); // do we need some special treatment for a closing brace?
			src << "}";
			indentLvl--;
		}
		void GlslWriter::VisitDeclStmt(DeclStmt* declStmt) {
			WriteIndentation();
			declStmt->GetDeclaration()->Accept(this);
			src << ";";
		}
		void GlslWriter::VisitExprStmt(ExprStmt* exprStmt) {
			WriteIndentation();
			exprStmt->GetExpression()->Accept(this);
			src << ";";
		}

		// Expression visit methods
		void GlslWriter::VisitAssignExpr(AssignExpr* assignExpr) {
			Expr* lvalue = assignExpr->GetLvalue();
			Expr* rvalue = assignExpr->GetRvalue();

			lvalue->Accept(this);
			src << " = "; // [TODO]: add other assignment operators later.
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
		void GlslWriter::VisitFloatConstExpr(FloatConstExpr* floatConstExpr) {
			const Token& floatConst = floatConstExpr->GetFloatConst();
			src << floatConst.lexeme;
		}
		void GlslWriter::VisitGroupExpr(GroupExpr* groupExpr) {
			src << "(";
			groupExpr->Accept(this);
			src << ")";
		}

		std::string GlslWriter::GetSrcCodeStr() const {
			return src.str();
		}

		void GlslWriter::ResetInternalState() {
			indentLvl = 0;
		}

		// Helper methods
		void GlslWriter::WriteFullySpecifiedType(const FullSpecType& fullSpecType) {
			if (!fullSpecType.qualifier.Empty()) {
				WriteTypeQualifier(fullSpecType.qualifier);
				src << " ";
			}
			src << fullSpecType.specifier.type.lexeme;
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

		void GlslWriter::WriteFunctionPrototype(const FunProto& funProto) {
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
		void GlslWriter::WriteFunctionParameterList(const FunParamList& funParamList) {
			const std::list<FunParam>& funParams = funParamList.GetParams();
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
			if (config.leftBraceOnSameLine)
				src << " {";
			else
				src << "\n{";
			src << "\n";
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