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


    // CAMERA
    WantedArmLength = CameraBoom->TargetArmLength;
    
    StanceArmLengthTransitionDuration = 0.25f;
    StandStanceArmLength = 300.f;
    CrouchStanceArmLength = 250.f;
    ProneStanceArmLength = 200.f;

    AimingArmLengthTransitionDuration = 0.1f;
    AimingArmLength = 180.f;

    // STANCE INIT
    StanceComponent = CreateDefaultSubobject<UStanceComponent>(TEXT("StanceComponent"));
    bIsChangeStanceInputPressed = false;
    bIsChangeStanceInputHolded = false;
    DelayForStanceHold = 0.2f;

    // FURTIVITY
    FurtivityComponent = CreateDefaultSubobject<UFurtivityComponent>(TEXT("FurtivityComponent"));

    // AIMING
    bIsAiming = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonCharacter::Tick(float DeltaSeconds)
{
    TickCameraArmLength(DeltaSeconds);

    DrawDebug();
}

void AThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up gameplay key bindings
    check(PlayerInputComponent);

    // MOVEMENT
    PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonCharacter::MoveRight);

    // CAMERA
    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &AThirdPersonCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AThirdPersonCharacter::LookUpAtRate);

    // STANCE
    PlayerInputComponent->BindAction("ChangeStance", IE_Pressed, this, &AThirdPersonCharacter::ChangeStanceInputPressed);
    PlayerInputComponent->BindAction("ChangeStance", IE_Released, this, &AThirdPersonCharacter::ChangeStanceInputReleased);

    // AIMING
    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AThirdPersonCharacter::Aim);
    PlayerInputComponent->BindAction("Aim", IE_Released, this, &AThirdPersonCharacter::StopAim);

    // ELECTRIC TOOL
    PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AThirdPersonCharacter::PickThrowableObject);
    PlayerInputComponent->BindAction("Throw", IE_Released, this, &AThirdPersonCharacter::ThrowObject);

    
}

FGenericTeamId AThirdPersonCharacter::GetGenericTeamId() const
{
    // TODO: This is temp: 0 is player, 1 is enemy, might need a proper "Faction" enum definition later
    return 0;
}


/*
*
*    --- MOVEMENT ---
*
*/
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

/*
*
*    --- CAMERA ---
*
*/
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

float AThirdPersonCharacter::GetWantedArmLengthFromStance(ECharacterStanceState CurrentStance)
{
    switch (CurrentStance)
    {
    case ECharacterStanceState::Stand:
        return StandStanceArmLength;
        break;
    case ECharacterStanceState::Crouch:
        return CrouchStanceArmLength;
        break;
    case ECharacterStanceState::Prone:
        return ProneStanceArmLength;
        break;
    }

    // INVALID PATH
    ensureMsgf(false, TEXT("Invalid Player Stance passed into parameters"));
    return 0.f;
}

void AThirdPersonCharacter::UpdateWantedCameraArmLength()
{
    float transitionDuration = 0.f;
    float wantedArmLength = 0.f;
    
    if (IsAiming())
    {
        wantedArmLength = AimingArmLength;
        transitionDuration = AimingArmLengthTransitionDuration;
    }
    else
    {
        wantedArmLength = GetWantedArmLengthFromStance(StanceComponent->GetCharacterStance());
        transitionDuration = StanceArmLengthTransitionDuration;
    }
        
    RequestCameraArmLengthChange(wantedArmLength, transitionDuration);
}

void AThirdPersonCharacter::RequestCameraArmLengthChange(float wantedArmLength, float transitionDuration)
{
    this->WantedArmLength = wantedArmLength;

    // Diff between current Arm Length and the wanted new one
    float ArmLengthDiff = WantedArmLength - CameraBoom->TargetArmLength;

    // The Arm Length speed (in m/s) needed in order to go from Current Arm Length to Wanted Arm Length in "ArmLengthTransitionDuration" seconds
    ArmLengthSpeed = ArmLengthDiff / transitionDuration;
}

void AThirdPersonCharacter::TickCameraArmLength(float DeltaSeconds)
{
    if (CameraBoom != nullptr)
    {
        float ArmLengthToAdd = ArmLengthSpeed * DeltaSeconds;
        if (!FMath::IsNearlyEqual(CameraBoom->TargetArmLength, WantedArmLength, FMath::Abs(ArmLengthToAdd) - 0.01f))
        {
            CameraBoom->TargetArmLength += ArmLengthToAdd;
        }
    }
}

/*
*
*    --- PLAYER STANCES ---
*
*/
void AThirdPersonCharacter::ChangeStanceInputPressed()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Input Pressed!"));

    bIsChangeStanceInputPressed = true;

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
    ECharacterStanceState CurrentStance = StanceComponent->GetCharacterStance();

    switch (CurrentStance)
    {
    case ECharacterStanceState::Stand:
        StanceComponent->ChangeStance(ECharacterStanceState::Crouch);
        break;
    case ECharacterStanceState::Crouch:
        StanceComponent->ChangeStance(ECharacterStanceState::Stand);
        break;
    case ECharacterStanceState::Prone:
        StanceComponent->ChangeStance(ECharacterStanceState::Crouch);
        break;
    case ECharacterStanceState::COUNT:
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

    ECharacterStanceState CurrentStance = StanceComponent->GetCharacterStance();

    switch (CurrentStance)
    {
    case ECharacterStanceState::Stand:
        StanceComponent->ChangeStance(ECharacterStanceState::Prone);
        break;
    case ECharacterStanceState::Crouch:
        StanceComponent->ChangeStance(ECharacterStanceState::Prone);
        break;
    case ECharacterStanceState::Prone:
        StanceComponent->ChangeStance(ECharacterStanceState::Stand);
        break;
    }

    bIsChangeStanceInputHolded = true;
}


/*
*
*    --- AIMING ---
*
*/
void AThirdPersonCharacter::Aim()
{
    bIsAiming = true;
    OnAimChanged(bIsAiming);
}

void AThirdPersonCharacter::StopAim()
{
    bIsAiming = false;
    OnAimChanged(bIsAiming);
}

/*
*
*    --- ELECTRIC TOOLS ---
*
*/
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


/*
*
*    --- DEBUG ---
*
*/
void AThirdPersonCharacter::DrawDebug()
{
    if (!bEnableDebugDraw) 
        return;

    DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + (100.f * this->GetActorForwardVector()), FColor::Red);
}