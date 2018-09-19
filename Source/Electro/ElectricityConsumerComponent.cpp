// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricityConsumerComponent.h"

#include "Public/DrawDebugHelpers.h"

// Sets default values for this component's properties
UElectricityConsumerComponent::UElectricityConsumerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
    CurrentElectricityLevelState = EElectricityLevelState::Normal;

	InitialElectricityEnergy = 1000.f;
	CurrentElectricityEnergy = InitialElectricityEnergy;
	DecreaseElectricityFactor = 1.f;
    
    LowElectricityRatio = 0.2f;
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
    OnTickDebug();


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
        ChangeState(EElectricityLevelState::Overflow);
    else if (CurrentElectricityRatio > 0.5f)
        ChangeState(EElectricityLevelState::Normal);
    else if (CurrentElectricityRatio > LowElectricityRatio)
        ChangeState(EElectricityLevelState::Medium);
    else if (CurrentElectricityRatio > 0.f)
        ChangeState(EElectricityLevelState::Low);
    else if (CurrentElectricityRatio <= 0.f)
        ChangeState(EElectricityLevelState::Empty);
}

void UElectricityConsumerComponent::ChangeState(EElectricityLevelState NewState)
{
    if (CurrentElectricityLevelState == NewState)
        return;

    if (NewState == EElectricityLevelState::Empty)
        ElectricityConsumer_OnSwitchChanged_Implementation(false);
    else if (CurrentElectricityLevelState == EElectricityLevelState::Empty)
        ElectricityConsumer_OnSwitchChanged_Implementation(true);

    CurrentElectricityLevelState = NewState;

    ElectricityConsumer_OnStateChanged.Broadcast();
}

void UElectricityConsumerComponent::ElectricityConsumer_OnSwitchChanged_Implementation(bool isSwitchedOn)
{
    ElectricityConsumer_OnSwitchChanged.Broadcast(isSwitchedOn);
}


// -------------------------------
// DEBUG PART
// -------------------------------

void UElectricityConsumerComponent::OnTickDebug()
{
    if (!enableDebug)
        return;

    DrawDebugString(GEngine->GetWorldFromContextObject(this), FVector::ZeroVector, *FString::Printf(TEXT("Energy %f"), this->GetCurrentElectricity()), this->GetOwner(), FColor::Red, 0.f);
}