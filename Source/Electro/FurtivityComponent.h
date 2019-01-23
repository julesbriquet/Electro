// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameFramework/MovementComponent.h"
#include "StanceComponent.h"
#include "AI_Enum.h"
#include "FurtivityComponent.generated.h"


USTRUCT(BlueprintType)
struct FFurtivityScoreData
{
    GENERATED_USTRUCT_BODY()

public:

    UPROPERTY(EditAnywhere)
    float MovingScore;
    UPROPERTY(EditAnywhere)
    float ImmobileScore;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELECTRO_API UFurtivityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFurtivityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
    float GetFurtivityScore(ESightType SightType) const;

		
    /*
    *
    *    --- MEMBER VARIABLES ---
    *
    */

protected:

    UStanceComponent* StanceComponent;

    UMovementComponent* MovementComponent;

protected:

    UPROPERTY(EditAnywhere, Category = Furtivity)
    FFurtivityScoreData SeenFurtivityScoreArray[ECharacterStanceState::COUNT];

    UPROPERTY(EditAnywhere, Category = Furtivity)
    FFurtivityScoreData SpottedFurtivityScoreArray[ECharacterStanceState::COUNT];
    
	
};
