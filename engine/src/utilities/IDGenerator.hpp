#ifndef ID_GENERATOR_HPP
#define ID_GENERATOR_HPP

#include "core/Types.hpp"

namespace Engine {

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
        static const ID id = generateTypeID();
        return id;
    }

    // Returns new ID each call.
    template<typename SubType = BaseType>
    static ID getInstanceID() {
        static ID id = 0;
        return id++;
    }

private:
    IDGenerator() = delete;
    ~IDGenerator() = delete;

    // This generates unique type-based IDs
    inline static ID generateTypeID()
    {
        static ID id = 0;
        return id++;
    }
};

} // Engine

#endif // !ID_GENERATOR_HPP