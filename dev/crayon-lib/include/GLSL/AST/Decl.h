#pragma once

#include "GLSL/Token.h"
#include "GLSL/Type.h"
#include "GLSL/AST/Stmt.h"

#include <memory>
#include <vector>

namespace crayon {
	namespace glsl {

		class Expr;
        class TransUnit;
        class FunDecl;
        class QualDecl;
        class VarDecl;
		class ArrayDecl;

        enum class DeclContext {
            EXTERNAL,
            BLOCK,
        };

        class DeclVisitor {
        public:
            virtual void VisitTransUnit(TransUnit* transUnit) = 0;
            virtual void VisitFunDecl(FunDecl* funDecl) = 0;
            virtual void VisitQualDecl(QualDecl* qualDecl) = 0;
			virtual void VisitVarDecl(VarDecl* varDecl) = 0;
			virtual void VisitArrayDecl(ArrayDecl* arrayDecl) = 0;
        };

        class Decl {
        public:
			virtual ~Decl() = default;
            virtual void Accept(DeclVisitor* declVisitor) = 0;
        };

        class TransUnit : public Decl {
		public:
			void Accept(DeclVisitor* declVisitor) override;

			// void AddFunDecl(std::shared_ptr<FunDecl> funDecl);
			// void AddQualDecl(std::shared_ptr<QualDecl> qualDecl);
			// void AddVarDecl(std::shared_ptr<VarDecl> varDecl);
			void AddDeclaration(std::shared_ptr<Decl> decl);

			const std::vector<std::shared_ptr<Decl>>& GetDeclarations();

		private:
			std::vector<std::shared_ptr<Decl>> decls;
		};

        class VarDecl : public Decl {
		public:
			VarDecl(const FullSpecType& varType, const Token& varName);
			virtual ~VarDecl() = default;

			void Accept(DeclVisitor* declVisitor) override;

			const FullSpecType& GetVariableType() const;
			const Token& GetVariableName() const;

			bool HasInitializerExpr() const;
			void SetInitializerExpr(std::shared_ptr<Expr> initExpr);
			std::shared_ptr<Expr> GetInitializerExpr() const;

		private:
			FullSpecType varType;
			Token varName;
			std::shared_ptr<Expr> initExpr;
		};
		class ArrayDecl : public VarDecl {
		public:
			ArrayDecl(const FullSpecType& varType, const Token& varName);
			virtual ~ArrayDecl() = default;

			void Accept(DeclVisitor* declVisitor) override;

			void AddDimension(std::shared_ptr<Expr> dimExpr);
			size_t GetDimensionCount() const;
			const std::vector<std::shared_ptr<Expr>>& GetDimensions() const;

		private:
			std::vector<std::shared_ptr<Expr>> dimensions;
		};

        class FunParam : public VarDecl {
		public:
			FunParam(const FullSpecType& paramType);
			FunParam(const FullSpecType& paramType, const Token& paramName);
			virtual ~FunParam() = default;

			bool HasName() const;
		};

		class FunParamList {
		public:
			void AddFunctionParameter(const FunParam& funParam);

			bool Empty() const;

			const std::list<FunParam>& GetParams() const;

		private:
			std::list<FunParam> funParams;
		};

		class FunProto {
		public:
			FunProto(const FullSpecType& retType, const Token& funName);
			FunProto(const FullSpecType& retType, const Token& funName, std::shared_ptr<FunParamList> params);

			const FullSpecType& GetReturnType() const;
			const Token& GetFunctionName() const;

			bool FunctionParameterListEmpty() const;
			const FunParamList& GetFunctionParameterList() const;

		private:
			FullSpecType retType;
			Token funName;

			std::shared_ptr<FunParamList> params;
		};

        class FunDecl : public Decl {
		public:
			FunDecl(std::shared_ptr<FunProto> funProto);
			FunDecl(std::shared_ptr<FunProto> funProto, std::shared_ptr<BlockStmt> stmts);
			virtual ~FunDecl() = default;

			void Accept(DeclVisitor* declVisitor) override;

            bool IsFunDecl() const;
            bool IsFunDef() const;
			bool IsFunBlockEmpty() const;

			const FunProto& GetFunctionPrototype() const;
            std::shared_ptr<BlockStmt> GetBlockStatement() const;

		private:
			std::shared_ptr<FunProto> funProto;
			std::shared_ptr<BlockStmt> stmts;
		};

        class QualDecl : public Decl {
		public:
			QualDecl(const TypeQual& qualifier);
			virtual ~QualDecl() = default;

			void Accept(DeclVisitor* declVisitor) override;

			const TypeQual& GetTypeQualifier() const;

		private:
			TypeQual qualifier;
		};
   
	}
}