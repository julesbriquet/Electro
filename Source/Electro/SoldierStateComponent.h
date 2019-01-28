// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AI_Enum.h"

#include "SoldierStateComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELECTRO_API USoldierStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoldierStateComponent();

    /*
    *
    *    --- FUNCTIONS ---
    *
    */

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


#pragma region SoldierState

public:
    UFUNCTION(BlueprintCallable, Category = PlayerStance)
    void ChangeState(ESoldierState NewState);

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = PlayerStance)
    void OnStateChanged(ESoldierState OldState, ESoldierState NewState);

#pragma endregion

public:
    FString GetDebugInfoString() const;

    /*
    *
    *    --- MEMBER VARIABLES ---
    *
    */

protected:

    UPROPERTY(VisibleAnywhere, Category = "Soldier State", meta = (AllowPrivateAccess = "true"))
    ESoldierState CurrentSoldierState;
	
};
