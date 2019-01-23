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

    for (int i = 0; i < PendingSightEnemiesList.Num();)
    {
        APawn* PendingEnemyInSight = PendingSightEnemiesList[i];

        if (CanSpotPawn(PendingEnemyInSight))
        {
            if (TransferPawn(PendingSightEnemiesList, SpottedEnemiesList, i))
                OnEnemySpotted(PendingEnemyInSight);
        }
        else if (CanSeePawn(PendingEnemyInSight))
        {
            if (TransferPawn(PendingSightEnemiesList, SeenEnemiesList, i))
                OnEnemySeen(PendingEnemyInSight);
        }
            
        else i++;
    }

    for (int i = 0; i < SeenEnemiesList.Num();)
    {
        APawn* SeenEnemy = SeenEnemiesList[i];

        if (CanSpotPawn(SeenEnemy))
        {
            if (TransferPawn(SeenEnemiesList, SpottedEnemiesList, i))
                OnEnemySpotted(SeenEnemy);
        }
        else i++;
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
                            PendingSightEnemiesList.AddUnique(PerceivedPawn);
                        else
                        {
                            if (!RemoveEnemyFromList(PendingSightEnemiesList, PerceivedPawn))
                            {
                                if (!RemoveEnemyFromList(SeenEnemiesList, PerceivedPawn))
                                    RemoveEnemyFromList(SpottedEnemiesList, PerceivedPawn);
                            }
                        }
                            
                    }
                }
            }
        }
    }
}


bool ASoldierAIController::RemoveEnemyFromList(TArray<APawn*>& RemovedFromList, APawn* EnemyToRemove)
{
    int removedNumber = RemovedFromList.RemoveSingleSwap(EnemyToRemove);

    return removedNumber > 0;
}

bool ASoldierAIController::TransferPawn(TArray<APawn*>& RemovedFromList, TArray<APawn*>& AddedToList, int RemovedPawnIndex)
{
    if (RemovedPawnIndex >= RemovedFromList.Num())
        return false;

    int pawnIndex = AddedToList.Add(RemovedFromList[RemovedPawnIndex]);
    RemovedFromList.RemoveAtSwap(RemovedPawnIndex);

    return true;
}


bool ASoldierAIController::CanSightPawn(APawn* Pawn, ESightType SightType)
{
    UFurtivityComponent* EnemyFurtivityComponent = Pawn->FindComponentByClass<UFurtivityComponent>();

    if (EnemyFurtivityComponent != nullptr)
    {
        FVector SoldierLocation = this->GetPawn()->GetActorLocation();
        FVector EnemyLocation = Pawn->GetActorLocation();

        float DistanceToEnemy = FVector::Distance(SoldierLocation, EnemyLocation);
        float FurtivityScoreInMeters = EnemyFurtivityComponent->GetFurtivityScore(SightType) * 100;

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Distance: %f, FurtivityScore: %f"), DistanceToEnemy, FurtivityScoreInMeters));

        return DistanceToEnemy <= this->SightConfig->SightRadius - FurtivityScoreInMeters;
    }

    return true;
}

bool ASoldierAIController::CanSeePawn(APawn* Pawn)
{
    return CanSightPawn(Pawn, ESightType::Seen);
}


bool ASoldierAIController::CanSpotPawn(APawn* Pawn)
{
    return CanSightPawn(Pawn, ESightType::Spotted);
}

void ASoldierAIController::OnEnemySeen(APawn* EnemySeen)
{
    if (SoldierStateComponent != nullptr)
        SoldierStateComponent->ChangeState(ESoldierState::Suspicious);

    if (SoldierFeedbackComponent != nullptr)
        SoldierFeedbackComponent->OnEnemySightFeedback(EnemySeen, ESightType::Seen);
}

void ASoldierAIController::OnEnemySpotted(APawn* EnemySpotted)
{
    if (SoldierStateComponent != nullptr)
    {
        SoldierStateComponent->ChangeState(ESoldierState::Fight);
    }

    if (SoldierFeedbackComponent != nullptr)
        SoldierFeedbackComponent->OnEnemySightFeedback(EnemySpotted, ESightType::Spotted);
}

FGenericTeamId ASoldierAIController::GetGenericTeamId() const
{
    // TODO: This is temp: 0 is player, 1 is enemy, might need a proper "Faction" enum definition later
    return 1;
}