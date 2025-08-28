#include "GLSL/Value.h"

#include <cassert>

namespace crayon {
    namespace glsl {

        ConstId ConstantTable::AddConstant(const ConstVal& constVal) {
            auto searchRes = constValSet.find(ConstantValue(constVal));
            // assert(searchRes != constValSet.end() && "The constant already exists in the constant table!");
            // 1. In case we're in the Release mode, just return the id of the already existing constant object.
            if (searchRes != constValSet.end())
                return searchRes->id;
            // 2. Allocate a new unique id and add the constant to the table.
            ConstId constId = idGen.GenerateUniqueId();
            constValMap.insert({constId, ConstantValue(constVal, constId)});
            constValSet.insert(ConstantValue(constVal, constId));
            return constId;
        }

        bool ConstantTable::ConstantExists(const ConstVal& constVal) const {
            auto searchRes = constValSet.find(ConstantValue(constVal));
            return searchRes != constValSet.end();
        }

        ConstVal ConstantTable::GetConstantValue(ConstId id) const {
            auto searchRes = constValMap.find(id);
            assert(searchRes != constValMap.end() && "Check if the constant exists first!");
            if (searchRes != constValMap.end())
                return ConstId(-1);
            return searchRes->second.value;
        }

        ConstId ConstantTable::GetConstantId(const ConstVal& constVal) const {
            auto searchRes = constValSet.find(ConstantValue(constVal));
            assert(searchRes != constValSet.end() && "Check if the constant exists first!");
            if (searchRes != constValSet.end())
                return ConstId(-1);
            return searchRes->id;
        }

        std::vector<ConstantValue> ConstantTable::GetConstants() const {
            std::vector<ConstantValue> constants(constValSet.size());
            size_t i = 0;
            for (const ConstantValue& constVal : constValSet) {
                constants[i++] = constVal;
            }
            return constants;
        }

        int ParseIntValue(std::string_view intVal) {
            if (intVal.size() == 1 && intVal[0] == '0')
                return 0;
            int radix = 0;
            if (intVal[0] == '0') {
                if (intVal[1] == 'x' || intVal[1] == 'X')
                    radix = 16;
                else
                    radix = 8;
            }
            // Could use a radix value of 0, in which case the function will auto-detect the number's base.
            int value = static_cast<int>(std::strtol(intVal.data(), nullptr, radix));
            return value;
        }
        unsigned int ParseUintValue(std::string_view uintVal) {
            if (uintVal.size() == 1 && uintVal[0] == '0')
                return 0;
            int radix = 0;
            if (uintVal[0] == '0') {
                if (uintVal[1] == 'x' || uintVal[1] == 'X')
                    radix = 16;
                else
                    radix = 8;
            }
            // Could use a radix value of 0, in which case the function will auto-detect the number's base.
            unsigned int value = static_cast<unsigned int>(std::strtoul(uintVal.data(), nullptr, radix));
            return value;
        }
        float ParseFloatValue(std::string_view floatVal) {
            float value = std::strtof(floatVal.data(), nullptr);
            return value;
        }
        double ParseDoubleValue(std::string_view doubleVal) {
            double value = std::strtod(doubleVal.data(), nullptr);
            return value;
        }

        void PrintConstantValue(std::ostream& out, const ConstantValue& constVal) {
            std::cout << "{"
                << "ID: " << constVal.id << ", ";
            std::cout << "VALUE: ";
            switch (constVal.constType) {
                case ConstType::INT:
                    std::cout << std::get<int>(constVal.value);
                    break;
                case ConstType::UINT:
                    std::cout << std::get<unsigned int>(constVal.value);
                    break;
                case ConstType::FLOAT:
                    std::cout << std::get<float>(constVal.value);
                    break;
                case ConstType::DOUBLE:
                    std::cout << std::get<double>(constVal.value);
                    break;
            }
            std::cout << ", ";
            std::cout << "SPIR-V ID: " << constVal.spirvId
                << "}";
        }

    }
}