#include "GLSL/CodeGen/GlslWriter.h"

namespace crayon
{
	namespace glsl
	{
		GlslWriter::GlslWriter(const GlslWriterConfig& config)
			: config(config)
		{
		}

		// Expression visit methods

		void GlslWriter::VisitBinaryExpr(Binary* binaryExpr)
		{
		}

		void GlslWriter::VisitIntConstExpr(IntConst* intConstExpr)
		{
		}

		void GlslWriter::VisitGroupExpr(GroupExpr* groupExpr)
		{
		}

		// Stmt visit methods

		void GlslWriter::VisitQualDecl(QualDecl* qualDecl)
		{
			const TypeQual& typeQual = qualDecl->GetTypeQualifier();
			WriteTypeQualifier(typeQual);
			src << ";";
		}

		void GlslWriter::VisitVarDecl(VarDecl* varDecl)
		{
			const FullSpecType& fullSpecType = varDecl->GetFullySpecifiedType();
			const Token& identifier = varDecl->GetIdentifier();

			WriteFullySpecifiedType(fullSpecType);
			src << " " << identifier.lexeme;
			src << ";";
		}

		void GlslWriter::VisitFunDecl(FunDecl* funDecl)
		{
			const FunProto& funProto = funDecl->GetFunctionPrototype();
			WriteFunctionPrototype(funProto);
			src << ";";
		}

		void GlslWriter::VisitFunDef(FunDef* funDef)
		{
			const FunProto& funProto = funDef->GetFunctionPrototype();
			WriteFunctionPrototype(funProto);

			std::shared_ptr<BlockStmt> funStmts = funDef->GetBlockStatement();
			VisitBlockStmt(funStmts.get());
		}

		void GlslWriter::VisitExtDeclList(ExtDeclList* extDeclList)
		{
			for (std::shared_ptr<Stmt> decl : extDeclList->GetDeclarations())
			{
				decl->Accept(this);
				src << "\n";
			}
		}

		void GlslWriter::VisitBlockStmt(BlockStmt* blockStmt)
		{
			WriteOpeningBlockBrace();

			// TODO
			
			// WriteClosingBlockBracer();
			src << "}";
		}

		std::string GlslWriter::GetSrcCodeStr() const
		{
			return src.str();
		}

		void GlslWriter::WriteFullySpecifiedType(const FullSpecType& fullSpecType)
		{
			if (fullSpecType.qualifier.has_value())
			{
				WriteTypeQualifier(fullSpecType.qualifier.value());
				src << " ";
			}
			src << fullSpecType.specifier.type.lexeme;
		}
		void GlslWriter::WriteTypeQualifier(const TypeQual& typeQual)
		{
			if (!typeQual.layout.empty())
			{
				WriteLayoutQualifier(typeQual.layout);
				src << " ";
			}
			if (typeQual.storage.has_value())
			{
				src << typeQual.storage.value().lexeme;
				src << " ";
			}

			// TODO: add more qualifiers

			src.seekp(-1, src.cur);
		}
		void GlslWriter::WriteLayoutQualifier(const std::list<LayoutQualifier>& layoutQualifiers)
		{
			src << "layout (";
			for (const LayoutQualifier& qualifier : layoutQualifiers)
			{
				src << qualifier.name.lexeme;
				if (qualifier.value.has_value())
				{
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
			const FullSpecType& fullSpecType = funProto.GetFullySpecifiedType();
			const Token& identifier = funProto.GetIdentifier();

			WriteFullySpecifiedType(fullSpecType);
			src << " " << identifier.lexeme;

			src << "(";
			if (!funProto.FunctionParameterListEmpty())
			{
				WriteFunctionParameterList(funProto.GetFunctionParameterList());
			}
			src << ")";
		}
		void GlslWriter::WriteFunctionParameterList(const FunParamList& funParamList)
		{
			const std::list<FunParam>& funParams = funParamList.GetFunctionParameters();
			for (const FunParam& funParam : funParams)
			{
				const FullSpecType& fullSpecType = funParam.GetFullSpecType();

				WriteFullySpecifiedType(fullSpecType);
				src << " ";

				if (funParam.HasIdentifier())
				{
					const Token& identifier = funParam.GetIdentifier();
					src << identifier.lexeme;
				}

				src << ", ";
			}
			
			if (!funParams.empty())
				src.seekp(-2, src.cur); // to remove the last two characters: ", "
		}

		void GlslWriter::WriteOpeningBlockBrace()
		{
			if (config.leftBraceOnSameLine)
				src << " {";
			else
				src << "\n{";

			src << "\n";
		}
	}
}