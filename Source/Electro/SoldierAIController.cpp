// Fill out your copyright notice in the Description page of Project Settings.

#include "SoldierAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "FurtivityComponent.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>

ASoldierAIController::ASoldierAIController(const FObjectInitializer& ObjectInitializer)
    : AAIController(ObjectInitializer)
{
    // Creating SubObjects
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SoldierStateComponent = CreateDefaultSubobject<USoldierStateComponent>(TEXT("SoldierStateComponent"));
    SoldierFeedbackComponent = CreateDefaultSubobject<USoldierFeedbackComponent>(TEXT("SoldierFeedbackComponent"));

    // Sense Configuration
    PerceptionComponent->ConfigureSense(*SightConfig);

    SightConfig->SightRadius = 7000.f;
    SightConfig->LoseSightRadius = 7500.f;

    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

    // Delegate Creation 
    PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ASoldierAIController::OnPerceptionUpdated);
    
}

void ASoldierAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    for (int i = 0; i < EnemiesInSightList.Num();)
    {
        APawn* EnemyInSight = EnemiesInSightList[i];

        if (CanBeSpotted(EnemyInSight))
            AddSightedEnemyInSpottedList(i);
        else
            i++;
    }
}

void ASoldierAIController::PostRegisterAllComponents()
{
    Super::PostRegisterAllComponents();

    // cache SoldierStateComponent if not already set
    if (SoldierStateComponent == NULL || SoldierStateComponent->IsPendingKill() == true)
    {
        SoldierStateComponent = FindComponentByClass<USoldierStateComponent>();
    }
}

void ASoldierAIController::OnPerceptionUpdated(const TArray<AActor *>& UpdatedActors)
{
    for (AActor* PerceivedActor : UpdatedActors)
    {
        const FActorPerceptionInfo* PerceivedInfoAboutActor = PerceptionComponent->GetActorInfo(*PerceivedActor);
        
        if (PerceivedInfoAboutActor != nullptr)
        {
            for (FAIStimulus PerceivedActorStimulis : PerceivedInfoAboutActor->LastSensedStimuli)
            {
                // Is Sight Stimulis?
                if (PerceivedActorStimulis.Type == SightConfig->GetSenseID())
                {
                    APawn* PerceivedPawn = Cast<APawn>(PerceivedActor);
                    if (PerceivedPawn != nullptr)
                    {
                        if (PerceivedActorStimulis.WasSuccessfullySensed() && !PerceivedActorStimulis.IsExpired())
                            AddEnemyInSightList(PerceivedPawn);
                        else
                        {
                            if (!RemoveEnemyInSightList(PerceivedPawn))
                                RemoveEnemyInSpottedList(PerceivedPawn);
                        }
                            
                    }
                }
            }
        }
    }
}


bool ASoldierAIController::AddEnemyInSightList(APawn* EnemyToAdd)
{
    int addIndex = EnemiesInSightList.AddUnique(EnemyToAdd);    
    return addIndex != -1;
}

bool ASoldierAIController::RemoveEnemyInSightList(APawn* EnemyToRemove)
{
    int removedNumber = EnemiesInSightList.RemoveSingleSwap(EnemyToRemove);

    return removedNumber > 0;
}

bool ASoldierAIController::AddSightedEnemyInSpottedList(int SightedEnemyIndex)
{
    if (SightedEnemyIndex >= EnemiesInSightList.Num())
        return false;

    int enemyIndex = EnemiesSpottedList.Add(EnemiesInSightList[SightedEnemyIndex]);
    EnemiesInSightList.RemoveAtSwap(SightedEnemyIndex);

    OnEnemySpotted(EnemiesSpottedList[enemyIndex]);

    return true;
}

bool ASoldierAIController::RemoveEnemyInSpottedList(APawn* EnemyToRemove)
{
    int removedNumber = EnemiesSpottedList.RemoveSingleSwap(EnemyToRemove);

    return removedNumber > 0;
}

bool ASoldierAIController::CanBeSpotted(APawn* Enemy)
{
    UFurtivityComponent* EnemyFurtivityComponent = Enemy->FindComponentByClass<UFurtivityComponent>();
    if (EnemyFurtivityComponent != nullptr)
    {
        FVector SoldierLocation = this->GetPawn()->GetActorLocation();
        FVector EnemyLocation = Enemy->GetActorLocation();

        float DistanceToEnemy = FVector::Distance(SoldierLocation, EnemyLocation);
        float FurtivityScoreInMeters = EnemyFurtivityComponent->GetFurtivityScore() * 100;

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Distance: %f, FurtivityScore: %f"), DistanceToEnemy, FurtivityScoreInMeters));

        return DistanceToEnemy <= this->SightConfig->SightRadius - FurtivityScoreInMeters;
    }

    // TODO
    return true;
}

void ASoldierAIController::OnEnemySpotted(APawn* EnemySpotted)
{
    if (SoldierStateComponent != nullptr)
    {
        SoldierStateComponent->ChangeState(ESoldierState::Fight);
    }

    if (SoldierFeedbackComponent != nullptr)
        SoldierFeedbackComponent->OnEnemySpottedFeedback(EnemySpotted);
}

FGenericTeamId ASoldierAIController::GetGenericTeamId() const
{
    // TODO: This is temp: 0 is player, 1 is enemy, might need a proper "Faction" enum definition later
    return 1;
}