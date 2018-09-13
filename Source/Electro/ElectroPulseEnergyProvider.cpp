// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectroPulseEnergyProvider.h"

#include <Components/SphereComponent.h>
#include <GameFramework/Actor.h>
#include <Public/TimerManager.h>

#include "ElectricityConsumerComponent.h"

// Sets default values for this component's properties
UElectroPulseEnergyProvider::UElectroPulseEnergyProvider()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
    PulseStartDelay = 5.f;
    ElectricityEnergy = 100.f;
    PulseNumber = 1;
    DelayBetweenPulse = 2.f;

    ElectroPulseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ElectroPulseSphere"));
}


// Called when the game starts
void UElectroPulseEnergyProvider::BeginPlay()
{
	Super::BeginPlay();

	// ...
    GetOwner()->GetWorldTimerManager().SetTimer(PulseTimer, this, &UElectroPulseEnergyProvider::OnElectricityPulse, DelayBetweenPulse, true, PulseStartDelay);
}

void UElectroPulseEnergyProvider::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetOwner()->GetWorldTimerManager().ClearTimer(PulseTimer);

    Super::EndPlay(EndPlayReason);
}


// Called every frame
void UElectroPulseEnergyProvider::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UElectroPulseEnergyProvider::OnElectricityPulse()
{
    PulseNumber--;
    if (PulseNumber <= 0)
        GetOwner()->GetWorldTimerManager().ClearTimer(PulseTimer);

    TArray<AActor*> OverlappingActorArray;
    ElectroPulseSphere->GetOverlappingActors(OverlappingActorArray);

    for (AActor* OverlappingActor : OverlappingActorArray)
    {
        TArray<UElectricityConsumerComponent*> ElectricityConsumerComponentList;
        OverlappingActor->GetComponents<UElectricityConsumerComponent>(ElectricityConsumerComponentList);

        for (UElectricityConsumerComponent* ElectricityConsumerComponent : ElectricityConsumerComponentList)
        {
            if (ElectricityConsumerComponent->IsActive())
                ElectricityConsumerComponent->UpdateEnergy(ElectricityEnergy);
        }
    }
}