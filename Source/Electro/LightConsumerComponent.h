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
	
protected:

	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
private:
	TArray<class ULightComponent*> LightComponentList;

	TArray<int> InitialLightIntensityList;
	
};
