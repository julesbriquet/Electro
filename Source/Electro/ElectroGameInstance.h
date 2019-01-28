// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ElectroGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRO_API UElectroGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
        UElectroGameInstance(const FObjectInitializer& ObjectInitializer);	
};
