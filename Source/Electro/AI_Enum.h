
#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class ESightType : uint8
{
    Seen,
    Spotted,
    COUNT,
};

UENUM(BlueprintType)
enum class ESoldierState : uint8
{
    Idle,
    Suspicious,
    Vigilant,
    Alert,
    Fight,
    COUNT
};


template<typename T>
static FString EnumToString(const FString& enumName, const T value)
{
    UEnum* pEnum = FindObject<UEnum>(ANY_PACKAGE, *enumName);
    return *(pEnum ? pEnum->GetNameStringByIndex(static_cast<uint8>(value)) : "null");
}