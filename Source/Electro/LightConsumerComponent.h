// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElectricityConsumerComponent.h"
#include "LightConsumerComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Electricity), meta = (BlueprintSpawnableComponent))
class ELECTRO_API ULightConsumerComponent : public UElectricityConsumerComponent
{
	GENERATED_BODY()


public:
    // Sets default values for this component's properties
    ULightConsumerComponent();

protected:

	void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
private:
	UPROPERTY(VisibleAnywhere, Category = "Electricity")
	TArray<class ULightComponent*> LightComponentList;

	TArray<int> InitialLightIntensityList;
	
};
