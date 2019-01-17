// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "SoldierStateComponent.h"
#include "SoldierFeedbackComponent.h"

#include "SoldierAIController.generated.h"


/**
 * 
 */
UCLASS()
class ELECTRO_API ASoldierAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    ASoldierAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /*
    *
    *    --- FUNCTIONS ---
    *
    */

    //~ Begin AActor Interface
public:
    virtual void Tick(float DeltaTime) override;
    virtual void PostRegisterAllComponents() override;
    //~ End AActor Interface
	
#pragma region Perception

protected:
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

#pragma endregion

#pragma region Perception_Sight

protected:
    bool AddEnemyInSightList(APawn* EnemyToAdd);
    bool RemoveEnemyInSightList(APawn* EnemyToRemove);

    bool AddSightedEnemyInSpottedList(int SightedEnemyIndex);
    bool RemoveEnemyInSpottedList(APawn* EnemyToRemove);

    bool CanBeSpotted(APawn* Enemy);
    
    void OnEnemySpotted(APawn* EnemySpotted);

#pragma endregion


protected:
    FGenericTeamId GetGenericTeamId() const override;


    /*
    *
    *    --- MEMBER VARIABLES ---
    *
    */
protected:
    UAISenseConfig_Sight* SightConfig;

    UPROPERTY(VisibleDefaultsOnly, Category = AI)
    USoldierStateComponent* SoldierStateComponent;

    UPROPERTY(VisibleDefaultsOnly, Category = AI)
    USoldierFeedbackComponent* SoldierFeedbackComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
    TArray<APawn*> EnemiesInSightList;

    UPROPERTY(VisibleAnywhere, Category = AI)
    TArray<APawn*> EnemiesSpottedList;
};
