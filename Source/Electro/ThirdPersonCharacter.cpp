// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include <Public/TimerManager.h>

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "DrawDebugHelpers.h"

#include "GameFramework/ProjectileMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// ARawThirdPersonCharacter

AThirdPersonCharacter::AThirdPersonCharacter()
{
    SetActorTickEnabled(true);

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

                                                // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


    // STANCE INIT
    bIsChangeStanceInputPressed = false;
    CurrentStance = EPlayerStanceState::Stand;
    DelayForStanceHold = 0.2f;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonCharacter::Tick(float DeltaSeconds)
{
    DrawDebug();
}

void AThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up gameplay key bindings
    check(PlayerInputComponent);

    PlayerInputComponent->BindAction("ChangeStance", IE_Pressed, this, &AThirdPersonCharacter::ChangeStanceInputPressed);
    PlayerInputComponent->BindAction("ChangeStance", IE_Released, this, &AThirdPersonCharacter::ChangeStanceInputReleased);

    PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AThirdPersonCharacter::PickThrowableObject);
    PlayerInputComponent->BindAction("Throw", IE_Released, this, &AThirdPersonCharacter::ThrowObject);

    PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonCharacter::MoveRight);

    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &AThirdPersonCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AThirdPersonCharacter::LookUpAtRate);
}

void AThirdPersonCharacter::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AThirdPersonCharacter::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AThirdPersonCharacter::ChangeStanceInputPressed()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Input Pressed!"));

    bIsChangeStanceInputPressed = true;
    //StanceBeforeInputPressed = CurrentStance;

    //ChangeStancePressed();

    GetWorldTimerManager().SetTimer(TimeSinceStancePressed, this, &AThirdPersonCharacter::ChangeStanceInputHold, DelayForStanceHold, false, DelayForStanceHold);
}

void AThirdPersonCharacter::ChangeStanceInputReleased()
{
    if (!bIsChangeStanceInputHolded)
        ChangeStancePressed();

    bIsChangeStanceInputPressed = false;
    bIsChangeStanceInputHolded = false;
    //StanceBeforeInputPressed = CurrentStance;

    GetWorldTimerManager().ClearTimer(TimeSinceStancePressed);
}

void AThirdPersonCharacter::ChangeStancePressed()
{
    switch (CurrentStance)
    {
    case EPlayerStanceState::Stand:
        ChangeStance(CurrentStance, EPlayerStanceState::Crouch);
        break;
    case EPlayerStanceState::Crouch:
        ChangeStance(CurrentStance, EPlayerStanceState::Stand);
        break;
    case EPlayerStanceState::Prone:
        ChangeStance(CurrentStance, EPlayerStanceState::Crouch);
        break;
    case EPlayerStanceState::Invalid:
        break;
    default:
        break;
    }
}

void AThirdPersonCharacter::ChangeStanceInputHold()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Input Hold!"));

    if (!bIsChangeStanceInputPressed)
        return;

    switch (CurrentStance)
    {
    case EPlayerStanceState::Stand:
        ChangeStance(CurrentStance, EPlayerStanceState::Prone);
        break;
    case EPlayerStanceState::Crouch:
        ChangeStance(CurrentStance, EPlayerStanceState::Prone);
        break;
    case EPlayerStanceState::Prone:
        ChangeStance(CurrentStance, EPlayerStanceState::Stand);
        break;
    
    }

    bIsChangeStanceInputHolded = true;
}

void AThirdPersonCharacter::ChangeStance(EPlayerStanceState OldStance, EPlayerStanceState NewStance)
{
    CurrentStance = NewStance;
}

void AThirdPersonCharacter::PickThrowableObject()
{
    if (EquipedThrowableObject != NULL)
    {
        UWorld* const CurrentWorld = GetWorld();
        if (CurrentWorld != NULL)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = Instigator;

            FVector SpawnLocation = this->GetActorLocation() + (100.f * this->GetActorForwardVector());
            FRotator SpawnRotation = this->GetActorRotation();

            SpawnedThrowableObject = CurrentWorld->SpawnActor<AActor>(EquipedThrowableObject, SpawnLocation, SpawnRotation, SpawnParams);
        
            SpawnedThrowableObject->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
        }
    }
}

void AThirdPersonCharacter::ThrowObject()
{
    if (SpawnedThrowableObject != nullptr)
    {
        SpawnedThrowableObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        TArray<UProjectileMovementComponent*> ProjectileComponentList;
        SpawnedThrowableObject->GetComponents<UProjectileMovementComponent>(ProjectileComponentList);

        for (UProjectileMovementComponent* ProjectileComponent : ProjectileComponentList)
        {
            ProjectileComponent->SetActive(true);
        }

        SpawnedThrowableObject = nullptr;
    }
}

void AThirdPersonCharacter::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AThirdPersonCharacter::MoveRight(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector 
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}

void AThirdPersonCharacter::DrawDebug()
{
    if (!bEnableDebugDraw) 
        return;

    DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + (100.f * this->GetActorForwardVector()), FColor::Red);
}