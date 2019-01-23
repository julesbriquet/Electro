// Fill out your copyright notice in the Description page of Project Settings.

#include "FurtivityComponent.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UFurtivityComponent::UFurtivityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFurtivityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
    StanceComponent = GetOwner()->FindComponentByClass<UStanceComponent>();
    MovementComponent = GetOwner()->FindComponentByClass<UMovementComponent>();

    // ASSERT
    ensureMsgf(StanceComponent != nullptr, TEXT("[FurtivityComponent] Stance Component is not present in the actor."));
    ensureMsgf(MovementComponent != nullptr, TEXT("[FurtivityComponent] Movement Component is not present in the actor."));
}


// Called every frame
void UFurtivityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UFurtivityComponent::GetFurtivityScore(ESightType SightType) const
{
    if (MovementComponent == nullptr || StanceComponent == nullptr)
        return 0.0f;


    float FurtivityScore = 0.f;

    ECharacterStanceState CurrentStance = StanceComponent->GetCharacterStance();
    bool isOwnerMoving = MovementComponent->Velocity.Size() > 0.f;

    FFurtivityScoreData FurtivityScoreStance;
    
    if (SightType == ESightType::Seen)
        FurtivityScoreStance = SeenFurtivityScoreArray[(int)CurrentStance];
    else if (SightType == ESightType::Spotted)
        FurtivityScoreStance = SpottedFurtivityScoreArray[(int)CurrentStance];

    if (isOwnerMoving)
        FurtivityScore = FurtivityScoreStance.MovingScore;
    else
        FurtivityScore = FurtivityScoreStance.ImmobileScore;


    return FurtivityScore;
}

