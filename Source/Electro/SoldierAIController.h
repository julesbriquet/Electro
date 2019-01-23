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

    typedef void (ASoldierAIController::*TransferCallbackMethod)(APawn* Pawn);

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
    bool RemoveEnemyFromList(TArray<APawn*>& RemovedFromList, APawn* EnemyToRemove);

    bool TransferPawn(TArray<APawn*>& RemovedFromList, TArray<APawn*>& AddedToList, int RemovedPawnIndex);
    

    bool CanSightPawn(APawn* Pawn, ESightType SightType);
    bool CanSeePawn(APawn* Pawn);
    bool CanSpotPawn(APawn* Pawn);
    
    
    void OnEnemySeen(APawn* EnemySeen);
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
    TArray<APawn*> PendingSightEnemiesList;

    UPROPERTY(VisibleAnywhere, Category = AI)
    TArray<APawn*> SeenEnemiesList;

    UPROPERTY(VisibleAnywhere, Category = AI)
    TArray<APawn*> SpottedEnemiesList;
};
