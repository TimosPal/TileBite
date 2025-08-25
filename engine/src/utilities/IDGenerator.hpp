#ifndef ID_GENERATOR_HPP
#define ID_GENERATOR_HPP

#include "core/Types.hpp"
#include "core/pch.hpp"

namespace TileBite {

// Helper macros for easier syntax (template keyword can be accidentaly ommited)
#define GET_TYPE_ID(Base, Sub) (TileBite::IDGenerator<Base>::template getTypeID<Sub>())
#define GET_INSTANCE_ID(Base, Sub) (TileBite::IDGenerator<Base>::template getInstanceID<Sub>())
#define GET_TYPE_NAME(Base, ID) (TileBite::IDGenerator<Base>::getTypeName(ID))

/*
Generator used to produce unique or instance based IDs.
IDs are not constexpr so sadly they can not be used in switch statements.
*/
template<typename BaseType>
class IDGenerator {
public:

    // Returns same ID each call.
    template<typename SubType = BaseType>
    inline static const ID getTypeID()
    {
        static const ID id = generateTypeID(typeid(SubType).name());
        return id;
    }

    // Returns new ID each call.
    template<typename SubType = BaseType>
    static ID getInstanceID()
    {
        static ID id = 0;
        return id++;
    }

	// Returns the name of the type associated with the ID
	static std::string getTypeName(ID id)
	{
		auto it = s_idToNameMap.find(id);
		if (it != s_idToNameMap.end())
		{
			return it->second;
		}
		return "Unknown ID";
	}

private:
    IDGenerator() = delete;
    ~IDGenerator() = delete;

    // This generates unique type-based IDs
    inline static ID generateTypeID(std::string className)
    {
        static ID id = 0;
        ID newID = id++;
		s_idToNameMap[newID] = className;
        return newID;
    }

    // Optional: Map IDs to names for debugging purposes
    inline static std::unordered_map<ID, std::string> s_idToNameMap = {};
};

} // TileBite

#endif // !ID_GENERATOR_HPP