// Fill out your copyright notice in the Description page of Project Settings.

#include "SoldierStateComponent.h"


// Sets default values for this component's properties
USoldierStateComponent::USoldierStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USoldierStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USoldierStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void USoldierStateComponent::ChangeState(ESoldierState NewState)
{
    ESoldierState OldState = CurrentSoldierState;

    CurrentSoldierState = NewState;

    OnStateChanged(OldState, NewState);
}

FString USoldierStateComponent::GetDebugInfoString() const
{
    FString CurrentSoldierStateName = EnumToString<ESoldierState>("ESoldierState", CurrentSoldierState);

    FString FORMAT_STRING = TEXT("{white}Soldier State: {yellow}" + CurrentSoldierStateName + "\n");

    return FORMAT_STRING;
}