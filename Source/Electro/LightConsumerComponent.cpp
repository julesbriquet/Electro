// Fill out your copyright notice in the Description page of Project Settings.

#include "LightConsumerComponent.h"
#include "Components/LightComponent.h"
#include "GameFramework/Actor.h"


// Called when the game starts
void ULightConsumerComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->GetComponents<ULightComponent>(LightComponentList);

	if (ensureMsgf(LightComponentList.Num() > 0, TEXT("Invalid Number of LightComponent (%d) for Actor %s"), LightComponentList.Num(), *(GetOwner()->GetName())))
	{
		for (ULightComponent* LightComponent : LightComponentList)
		{
			InitialLightIntensityList.Add(LightComponent->Intensity);
		}
	}
}

void ULightConsumerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsActive())
	{
		float EnergyRatio = GetCurrentElectricity() / InitialElectricityEnergy;

		for (int i = 0; i < LightComponentList.Num(); ++i)
			LightComponentList[i]->SetIntensity(EnergyRatio * InitialLightIntensityList[i]);
	}
}