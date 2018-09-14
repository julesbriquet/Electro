// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "ElectroPulseEnergyProvider.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELECTRO_API UElectroPulseEnergyProvider : public USphereComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UElectroPulseEnergyProvider();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    /* Called each time the Energy Provider is doing an Electricity Pulse */
    UFUNCTION(BlueprintCallable, Category = "Electricty")
    void OnElectricityPulse();

protected:
    UPROPERTY(EditAnywhere, Category = "Electricity")
    float PulseStartDelay;

    UPROPERTY(EditAnywhere, Category = "Electricity")
    float ElectricityEnergy;

    UPROPERTY(EditAnywhere, Category = "Electricity")
    int PulseNumber;

    UPROPERTY(EditAnywhere, Category = "Electricity")
    float DelayBetweenPulse;

private:

    FTimerHandle PulseTimer;
};
