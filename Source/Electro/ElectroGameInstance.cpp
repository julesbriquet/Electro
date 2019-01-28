// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectroGameInstance.h"
#include "GameplayDebuggerCategoryT.h"
#include "SoldierStateComponent.h"
#include "SoldierFeedbackComponent.h"

UElectroGameInstance::UElectroGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    FGameplayDebuggerCategoryT<USoldierStateComponent>::Register(TEXT("Soldier State"), 6);
}