// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectroGameModeBase.h"
#include "ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

AElectroGameModeBase::AElectroGameModeBase()
{
    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCharacter/Blueprints/ThirdPersonCharacter_BP"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}
