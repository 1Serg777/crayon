#pragma once

#include "GLSL/Type.h"

#include <memory>
#include <vector>

namespace crayon {
	namespace glsl {

		class BlockStmt;

		class StmtVisitor {
		public:
			virtual void VisitBlockStmt(BlockStmt* blockStmt) = 0;
		};

		class Stmt {
		public:
			virtual void Accept(StmtVisitor* stmtVisitor) = 0;
		};

		class BlockStmt : public Stmt {
		public:
			void Accept(StmtVisitor* stmtVisitor) override;

			void AddStmt(std::shared_ptr<Stmt> stmt);

		private:
			std::vector<std::shared_ptr<Stmt>> stmts;
		};
	}
}