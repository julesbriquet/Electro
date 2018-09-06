// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricityConsumerComponent.h"


// Sets default values for this component's properties
UElectricityConsumerComponent::UElectricityConsumerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	InitialElectricityEnergy = 1000.f;
	CurrentElectricityEnergy = InitialElectricityEnergy;
	DecreaseElectricityFactor = 1.f;
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
}
