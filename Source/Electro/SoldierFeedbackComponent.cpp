// Fill out your copyright notice in the Description page of Project Settings.

#include "SoldierFeedbackComponent.h"


// Sets default values for this component's properties
USoldierFeedbackComponent::USoldierFeedbackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USoldierFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USoldierFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USoldierFeedbackComponent::OnEnemySightFeedback(const APawn* EnemySpotted, ESightType SightType)
{
    SoldierFeedbackComponent_OnEnemySightFeedback.Broadcast(EnemySpotted, SightType);
}