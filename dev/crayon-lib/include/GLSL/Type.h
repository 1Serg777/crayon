#pragma once

#include "GLSL/Token.h"

#include <list>
#include <optional>

namespace crayon
{
	namespace glsl
	{
		class LayoutQualifier
		{
		public:

			LayoutQualifier(const Token& name);
			LayoutQualifier(const Token& name, int value);

			const Token& GetQualifierName() const;

			bool QualifierValueExists() const;
			int GetQualifierValue() const;

		private:

			Token name;
			std::optional<int> value;
		};

		class LayoutQualifierList
		{
		public:

			void AddLayoutQualifier(const LayoutQualifier& layoutQualifier);

		private:

			std::list<LayoutQualifier> qualifiers;
		};

		class StorageQualifier
		{
		public:
			
			StorageQualifier() = default;
			StorageQualifier(const Token& qualifier);

		private:

			Token qualifier;
		};

		class StorageQualifierList
		{
		public:

		private:

		};

		class TypeQual
		{
		public:

			TypeQual();

			void SetLayoutQualifier(std::shared_ptr<LayoutQualifierList> layoutQualifierList);

			void SetStorageQualifier(const StorageQualifier& storageQualifier);

			void SetInterpolationQualifier(const Token& interpolation);
			const Token& GetInterpolationQualifier() const;

		private:

			StorageQualifier storage;

			std::shared_ptr<LayoutQualifierList> layout;

			// [TODO] add more qualifiers:
			// Precision, Interpolation, Invariant, Precise

			Token interpolation;
		};

		class TypeSpec
		{
		public:

			TypeSpec() = default;
			TypeSpec(const Token& type);

			const Token& GetType() const;

		private:

			Token type;
		};

		struct FullSpecType
		{
			std::optional<TypeQual> qualifier;
			TypeSpec specifier;
		};
	}
}