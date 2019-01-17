// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"
#include "SoldierFeedbackComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSoldierFeedbackSpottedEnemy, const APawn*, SpottedEnemy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELECTRO_API USoldierFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoldierFeedbackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



public:
    void OnEnemySpottedFeedback(const APawn* EnemySpotted);

protected:
    UPROPERTY(BlueprintAssignable, Category = AI)
    FSoldierFeedbackSpottedEnemy SoldierFeedbackComponent_OnEnemySpottedFeedback;
	
};
