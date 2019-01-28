 //Fill out your copyright notice in the Description page of Project Settings
 //GameplayDebuggerCategoryT.h
#pragma once

#include "GameplayDebuggerCategory.h"
#include "GameplayDebugger.h"

 //Gameplay debugger category to show information when the gameplay debugger is shown in game
 //(apostrophe hotkey). This templatised class can be used for debugging any Actor Component.
 //Instantiate this class by calling the static Register function from your GameInstance,
 //implement a new function on your actor component with the signature: 
 //FString GetDebugInfoString()

template<typename T>
class FGameplayDebuggerCategoryT : public FGameplayDebuggerCategory
{
public:
    FGameplayDebuggerCategoryT();

    // Call this to register your category in the Gameplay Debugger
    static void Register(const FName& Name, int Hotkey);

    virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
    virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;
    virtual void OnDataPackReplicated(int32 DataPackId) override;

protected:
    static TSharedRef<FGameplayDebuggerCategory> MakeInstance();

    struct FDebugData
    {
        TArray<FString> DebugStrings;
        void Serialize(FArchive& archive);
    }
    DebugData;
};




///////////////////////////////////////////////////////////////////////////////
// FGameplayDebuggerCategoryT factory instantiation
///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline FGameplayDebuggerCategoryT<T>::FGameplayDebuggerCategoryT()
{
    SetDataPackReplication(&DebugData);
}

template<typename T>
inline void FGameplayDebuggerCategoryT<T>::Register(const FName& Name, int Hotkey)
{
    const auto STATE_FLAGS = EGameplayDebuggerCategoryState::EnabledInGame;

    auto& debugger = IGameplayDebugger::Get();
    auto creationDelegate = IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategoryT<T>::MakeInstance);

    debugger.RegisterCategory(Name, creationDelegate, STATE_FLAGS, Hotkey);
    debugger.NotifyCategoriesChanged();
}

template<typename T>
inline TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategoryT<T>::MakeInstance()
{
    return MakeShareable(new FGameplayDebuggerCategoryT<T>());
}

///////////////////////////////////////////////////////////////////////////////
// FGameplayDebuggerCategoryT
///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void FGameplayDebuggerCategoryT<T>::CollectData(APlayerController* Owner, AActor* DebugActor)
{
    DebugData.DebugStrings.Empty();
    if (!DebugActor) return;

    for (UActorComponent* component : DebugActor->GetComponents())
    {
        T* componentCasted = Cast<T>(component);
        if (componentCasted)
        {
            DebugData.DebugStrings.Add(componentCasted->GetDebugInfoString());
        }
    }

    APawn* DebugPawn = Cast<APawn>(DebugActor);
    if (DebugPawn)
    {
        AController* DebugActorController = Cast<AController>(DebugPawn->GetController());
        if (DebugActorController)
        {
            for (UActorComponent* component : DebugActorController->GetComponents())
            {
                T* componentCasted = Cast<T>(component);
                if (componentCasted)
                {
                    DebugData.DebugStrings.Add(componentCasted->GetDebugInfoString());
                }
            }
        }
    }
}

template<typename T>
inline void FGameplayDebuggerCategoryT<T>::OnDataPackReplicated(int32 DataPackId)
{
    MarkRenderStateDirty();
}

template<typename T>
inline void FGameplayDebuggerCategoryT<T>::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
    for (const FString& debugInfoString : DebugData.DebugStrings)
    {
        CanvasContext.Printf(TEXT("%s"), *debugInfoString);
    }
}


///////////////////////////////////////////////////////////////////////////////
// FGameplayDebuggerCategoryT::FDebugData
///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void FGameplayDebuggerCategoryT<T>::FDebugData::Serialize(FArchive& archive)
{
    for (auto debugString : DebugStrings)
    {
        archive << debugString;
    }
}