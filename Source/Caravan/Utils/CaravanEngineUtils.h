#pragma once

/**
 * Convert the value of an enum to a string.
 *
 * @param EnumValue
 *    The enumerated type value to convert to a string.
 *
 * @return
 *    The key/name that corresponds to the value in the enumerated type.
 */
namespace CaravanUtils
{
    template<typename T>
    FString EnumToString(const T EnumValue)
    {
        FString Name = StaticEnum<T>()->GetNameStringByValue(static_cast<__underlying_type(T)>(EnumValue));

        check(Name.Len() != 0);

        return Name;
    }
}

/*
* Find a component, or create it if not found.
* Can only be called from Actor constructor.
* 
* TODO: Move to CaravanBaseActor class…
*/
#define FindOrCreateComponent(ComponentClass, ComponentObject, ComponentName) \
    ComponentObject = FindComponentByClass< ComponentClass >(); \
    if (ComponentObject == NULL) \
    { \
    	ComponentObject = ObjInitializer.CreateDefaultSubobject< ComponentClass >(this, TEXT(ComponentName)); \
        AddOwnedComponent(ComponentObject); \
    }