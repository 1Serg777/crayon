#include "GLSL/Type.h"

namespace crayon
{
	namespace glsl
	{
		LayoutQualifier::LayoutQualifier(const Token& name)
			: name(name)
		{
		}
		LayoutQualifier::LayoutQualifier(const Token& name, int value)
			: name(name), value(value)
		{
		}

		const Token& LayoutQualifier::GetQualifierName() const
		{
			return name;
		}

		bool LayoutQualifier::QualifierValueExists() const
		{
			return value.has_value();
		}
		int LayoutQualifier::GetQualifierValue() const
		{
			if (!QualifierValueExists())
				return -1;
			return value.value();
		}

		StorageQualifier::StorageQualifier(const Token& qualifier)
			: qualifier(qualifier)
		{
		}

		TypeQual::TypeQual()
		{
			// storage = std::make_shared<StorageQualifierList>();
			layout = std::make_shared<LayoutQualifierList>();
		}

		void TypeQual::SetLayoutQualifier(std::shared_ptr<LayoutQualifierList> layoutQualifierList)
		{
			this->layout = layoutQualifierList;
		}

		void TypeQual::SetStorageQualifier(const StorageQualifier& storageQualifier)
		{
			this->storage = storageQualifier;
		}

		void TypeQual::SetInterpolationQualifier(const Token& interpolation)
		{
			this->interpolation = interpolation;
		}
		const Token& TypeQual::GetInterpolationQualifier() const
		{
			return this->interpolation;
		}

		TypeSpec::TypeSpec(const Token& type)
			: type(type)
		{
		}

		const Token& TypeSpec::GetType() const
		{
			return type;
		}
		
		void LayoutQualifierList::AddLayoutQualifier(const LayoutQualifier& layoutQualifier)
		{
			qualifiers.push_back(layoutQualifier);
		}
	}
}