// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElectricityConsumerComponent.generated.h"


UENUM (BlueprintType)
enum class EEnergyLevelState : uint8
{
    Normal,
    Medium,
    Low,
    Empty,
    Overflow,
    Invalid,
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnergyStateChanged);

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

	/* Return the Current Energy Level of the Electricity Consumer */
	UFUNCTION(BlueprintPure, Category = "Electricity")
	FORCEINLINE float GetCurrentElectricity() const { return CurrentElectricityEnergy; }

    UFUNCTION(BlueprintPure, Category = "Electricity")
    FORCEINLINE float GetCurrentElectricityRatio() const { return GetCurrentElectricity() / InitialElectricityEnergy; }

	/* Function to change the Current Energy of the Electricity Consumer*/
	UFUNCTION(BlueprintCallable, Category = "Electricity")
	void UpdateEnergy(float EnergyChange);
	
	UFUNCTION(BlueprintPure, Category = "Electricity")
	FORCEINLINE EEnergyLevelState GetCurrentEnergyState() const { return CurrentElectricityLevelState; }

protected:

	UPROPERTY(BlueprintAssignable, Category = "Electricity")
	FEnergyStateChanged EnergyConsumer_OnStateChanged;

    UFUNCTION(BlueprintCallable, Category = "Electricity")
    void ChangeState(EEnergyLevelState NewState);

    virtual void OnStateChanged_Implementation() {}

protected:

    UPROPERTY(VisibleAnywhere, Category = "Electricity")
    TEnumAsByte<EEnergyLevelState> CurrentElectricityLevelState;

	/* Current Amount of Electricity this Consumer have. */
	UPROPERTY(VisibleAnywhere, Category = "Electricity")
	float CurrentElectricityEnergy;
	
	UPROPERTY(EditAnywhere, Category = "Electricity")
	float InitialElectricityEnergy;

	/* Decrease Rate (in Unit/Seconds) of the Energy of this Electricity Consummer */
	UPROPERTY(EditAnywhere, Category = "Electricity")
	float DecreaseElectricityFactor;

    UPROPERTY(EditAnywhere, Category = "Electricity")
    float LowEnergyRatio;
};
