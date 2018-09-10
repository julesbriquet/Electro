// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricityConsumerComponent.h"


// Sets default values for this component's properties
UElectricityConsumerComponent::UElectricityConsumerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
    CurrentElectricityLevelState = EEnergyLevelState::Normal;

	InitialElectricityEnergy = 1000.f;
	CurrentElectricityEnergy = InitialElectricityEnergy;
	DecreaseElectricityFactor = 1.f;
    
    LowEnergyRatio = 0.2f;
}


// Called when the game starts
void UElectricityConsumerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentElectricityEnergy = InitialElectricityEnergy;
}


// Called every frame
void UElectricityConsumerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsActive())
	{
		UpdateEnergy(-DecreaseElectricityFactor * DeltaTime);
	}
}

void UElectricityConsumerComponent::UpdateEnergy(float EnergyChange)
{
    CurrentElectricityEnergy += EnergyChange;

    // Avoid Negative Values for Energy
    if (CurrentElectricityEnergy < 0.f)
        CurrentElectricityEnergy = 0.f;

    float CurrentElectricityRatio = GetCurrentElectricityRatio();

    if (CurrentElectricityRatio > 1.f)
        ChangeState(EEnergyLevelState::Overflow);
    else if (CurrentElectricityRatio > 0.5f)
        ChangeState(EEnergyLevelState::Normal);
    else if (CurrentElectricityRatio > LowEnergyRatio)
        ChangeState(EEnergyLevelState::Medium);
    else if (CurrentElectricityRatio > 0.f)
        ChangeState(EEnergyLevelState::Low);
    else if (CurrentElectricityRatio <= 0.f)
        ChangeState(EEnergyLevelState::Empty);
}

void UElectricityConsumerComponent::ChangeState(EEnergyLevelState NewState)
{
    if (CurrentElectricityLevelState == NewState)
        return;

    CurrentElectricityLevelState = NewState;
}