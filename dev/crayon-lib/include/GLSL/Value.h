#pragma once

#include "Utility.h"

#include <cstdint>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace crayon {
    namespace glsl {
        enum class ConstType {
            UNDEFINED = -1,
            INT,
            UINT,
            FLOAT,
            DOUBLE,
        };

        using ConstVal = std::variant<int, unsigned int, float, double>;
        using ConstId = uint32_t;
        using SpirvConstId = ConstId; // Should it be here?

        struct ConstantValue {
            ConstantValue()
                : value(value), id(-1), spirvId(-1), constType(ConstType::UNDEFINED) {}
            ConstantValue(const ConstVal& value)
                : value(value), id(-1), spirvId(-1), constType(static_cast<ConstType>(value.index())) {}
            ConstantValue(const ConstVal& value, ConstId id)
                : value(value), id(id), spirvId(-1), constType(static_cast<ConstType>(value.index())) {}

            bool operator==(const ConstantValue& other) const {
                return value == other.value;
                // return id == other.id;
            }

            ConstVal value;
            ConstId id;
            SpirvConstId spirvId; // Should it be here?
            ConstType constType;
        };
    }
}

namespace std {
    template <>
    struct hash<crayon::glsl::ConstantValue> {
        auto operator()(const crayon::glsl::ConstantValue& constVal) const -> size_t {
            return hash<crayon::glsl::ConstVal>{}(constVal.value);
        }
    };
}

namespace crayon {
	namespace glsl {

		enum class IntConstType {
            OCT = 8, // 00005, 01732, etc.
            DEC = 10, // 1, 12, 35, etc.
            HEX = 16, // 0x1A, 0X0, etc.
        };

        enum class FloatConstType {
            NORM, // 1.0, 0.53, .99, 35., etc.
            EXP, // 1e5, 0e-3, 0.15e2, etc.
        };

        class ConstantTable {
        public:
            ConstId AddConstant(const ConstVal& constVal);
            template<typename T>
            ConstId AddConstant(const T& val) {
                ConstVal constVal = val;
                return AddConstant(constVal);
            }

            bool ConstantExists(const ConstVal& constVal) const;
            template<typename T>
            bool ConstantExists(const T& t) {
                ConstVal constVal = t;
                return ConstantExists(constVal);
            }

            ConstVal GetConstantValue(ConstId id) const;

            ConstId GetConstantId(const ConstVal& constVal) const;
            template<typename T>
            ConstId GetConstantId(const T& t) {
                ConstVal constVal = t;
                return GetConstantId(constVal);
            }

            std::vector<ConstantValue> GetConstants() const;

        private:
            std::unordered_map<ConstId, ConstantValue> constValMap;
            std::unordered_set<ConstantValue> constValSet;
            SeqIdGenerator<ConstId> idGen;
        };

        int ParseIntValue(std::string_view intVal);
        unsigned int ParseUintValue(std::string_view uintVal);
        float ParseFloatValue(std::string_view floatVal);
        double ParseDoubleValue(std::string_view doubleVal);

        void PrintConstantValue(std::ostream& out, const ConstantValue& constVal);

	}
}