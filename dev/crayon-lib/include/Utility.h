#pragma once

#include <cassert>
#include <cstdint>
#include <stack>
#include <string_view>
#include <type_traits>

namespace crayon {

#define CLASS_DEFAULT_COPY(class_name)\
	class_name(const class_name& copy) = default;\
	class_name& operator=(const class_name& copy) = default

#define CLASS_DEFAULT_MOVE(class_name)\
	class_name(class_name&& move) = default;\
	class_name& operator=(class_name&& move) = default

#define CLASS_NO_COPY(class_name)\
	class_name(const class_name& copy) = delete;\
	class_name& operator=(const class_name& copy) = delete

#define CLASS_NO_MOVE(class_name)\
	class_name(class_name&& move) = delete;\
	class_name& operator=(class_name&& move) = delete

	bool FileExtCsl(std::string_view ext);

	// Only unsigned integer types work (i.e. size_t, uint32_t, uint64_t, etc.).
	// Id = 0 is considered invalid!
	template <typename IdType,
		      std::enable_if_t<std::is_unsigned_v<IdType>, bool> = true>
	class SeqIdGenerator {
	public:
		SeqIdGenerator() = default;
		~SeqIdGenerator() = default;
		CLASS_NO_COPY(SeqIdGenerator);
		CLASS_DEFAULT_MOVE(SeqIdGenerator);

		IdType GenerateUniqueId() {
			if (!freeIds.empty()) {
				IdType id = freeIds.top();
				freeIds.pop();
				return id;
			}
			return idCounter++;
		}
		void FreeUniqueId(IdType id) {
			assert(id > 0 && id < idCounter && "Invalid ID provided!");
			freeIds.push(id);
		}
		IdType GetLastGeneratedUniqueId() const {
			return idCounter - 1;
		}

	private:
		IdType idCounter{1}; // Stores the next Id TO BE generated.
		std::stack<IdType> freeIds;
	};

	size_t CalcDigitCount(size_t number);

}