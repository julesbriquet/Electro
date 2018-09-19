// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElectricityConsumerComponent.generated.h"


UENUM (BlueprintType)
enum class EElectricityLevelState : uint8
{
    Normal,
    Medium,
    Low,
    Empty,
    Overflow,
    Invalid,
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElectricityLevelStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FElectricitySwitchChanged, bool, isSwitchedOn);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELECTRO_API UElectricityConsumerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UElectricityConsumerComponent();

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Return the Current Electricity Level of the Electricity Consumer */
	UFUNCTION(BlueprintPure, Category = "Electricity")
	FORCEINLINE float GetCurrentElectricity() const { return CurrentElectricityEnergy; }

    UFUNCTION(BlueprintPure, Category = "Electricity")
    FORCEINLINE float GetCurrentElectricityRatio() const { return GetCurrentElectricity() / InitialElectricityEnergy; }

	/* Function to change the Current Energy of the Electricity Consumer*/
	UFUNCTION(BlueprintCallable, Category = "Electricity")
	void UpdateEnergy(float EnergyChange);
	
	UFUNCTION(BlueprintPure, Category = "Electricity")
	FORCEINLINE EElectricityLevelState GetCurrentEnergyState() const { return CurrentElectricityLevelState; }

protected:

	UPROPERTY(BlueprintAssignable, Category = "Electricity")
	FElectricityLevelStateChanged ElectricityConsumer_OnStateChanged;

    UFUNCTION(BlueprintCallable, Category = "Electricity")
    void ChangeState(EElectricityLevelState NewState);

    void ElectricityConsumer_OnSwitchChanged_Implementation(bool isSwitchedOn);

    UPROPERTY(BlueprintAssignable, Category = "Electricity")
    FElectricitySwitchChanged ElectricityConsumer_OnSwitchChanged;

protected:

    /* Energy Level State of this Electricity Component */
    UPROPERTY(VisibleAnywhere, Category = "Electricity")
    EElectricityLevelState CurrentElectricityLevelState;

	/* Current Amount of Electricity this Consumer have. */
	UPROPERTY(VisibleAnywhere, Category = "Electricity")
	float CurrentElectricityEnergy;
	
    /* The Initial Electricity that the Consumer have */
	UPROPERTY(EditAnywhere, Category = "Electricity")
	float InitialElectricityEnergy;

	/* Decrease Rate (in Unit/Seconds) of the Electricity of this Electricity Consummer */
	UPROPERTY(EditAnywhere, Category = "Electricity")
	float DecreaseElectricityFactor;

    /* The Electricity Ratio at which the Electricity Level of the Consumer is considered Low */
    UPROPERTY(EditAnywhere, Category = "Electricity")
    float LowElectricityRatio;

    // -------------------------------
    // DEBUG PART
    // -------------------------------

protected:
    virtual void OnTickDebug();

    UPROPERTY(EditAnywhere, Category = "Debug")
    bool enableDebug = false;
};
