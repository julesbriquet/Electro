// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StanceComponent.generated.h"

UENUM(BlueprintType)
enum class ECharacterStanceState : uint8
{
    Stand,
    Crouch,
    Prone,
    COUNT,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELECTRO_API UStanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStanceComponent();

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


    UFUNCTION(BlueprintCallable, Category = CharacterStance)
    void ChangeStance(ECharacterStanceState NewStance);

    UFUNCTION(BlueprintImplementableEvent, Category = CharacterStance)
    void OnStanceChanged(ECharacterStanceState OldStance, ECharacterStanceState NewStance);
	
    /*
    *
    *    --- GETTER/SETTER ---
    *
    */

public:
    UFUNCTION(BlueprintPure, Category = CharacterStance)
    FORCEINLINE ECharacterStanceState GetCharacterStance() const { return CurrentStance; }


protected:
    UPROPERTY(VisibleAnywhere, Category = CharacterStance, meta = (AllowPrivateAccess = "true"))
    ECharacterStanceState  CurrentStance;
};
