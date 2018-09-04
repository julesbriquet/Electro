// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElectricityConsumerComponent.generated.h"


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

		
protected:

	/* Current Amount of Electricity this Consumer have. */
	UPROPERTY(VisibleAnywhere, Category = "Electricity")
	float CurrentElectricityEnergy;
	
	UPROPERTY(EditAnywhere, Category = "Electricity")
	float InitialElectricityEnergy;

	/* Decrease Rate (in Unit/Seconds) of the Energy of this Electricity Consummer */
	UPROPERTY(EditAnywhere, Category = "Electricity")
	float DecreaseElectricityFactor;

};
