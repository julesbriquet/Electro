// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/AIModule/Classes/GenericTeamAgentInterface.h"
#include "ThirdPersonCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerStanceState : uint8
{
    Stand,
    Crouch,
    Prone,
    COUNT
};

UCLASS(config = Game)
class ELECTRO_API AThirdPersonCharacter : public ACharacter, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    AThirdPersonCharacter();

    /*
    *
    *    --- FUNCTIONS ---
    *
    */

#pragma region PawnInterface
    
protected:
    virtual void Tick(float DeltaSeconds) override;

    
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma endregion

#pragma region GenericTeamAgentInterface

protected:
    virtual FGenericTeamId GetGenericTeamId() const override;

#pragma endregion

#pragma region Movement

    /** Called for forwards/backward input */
    void MoveForward(float Value);

    /** Called for side to side input */
    void MoveRight(float Value);

#pragma endregion

#pragma region Camera

    /**
    * Called via input to turn at a given rate.
    * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
    */
    void TurnAtRate(float Rate);

    /**
    * Called via input to turn look up/down at a given rate.
    * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
    */
    void LookUpAtRate(float Rate);

    UFUNCTION(BlueprintCallable, Category = Camera)
    float GetWantedArmLengthFromStance(EPlayerStanceState PlayerStance);

    UFUNCTION(BlueprintCallable, Category = Camera)
    void UpdateWantedCameraArmLength();

    UFUNCTION(BlueprintCallable, Category = Camera)
    void RequestCameraArmLengthChange(float wantedArmLength , float transitionDuration);

    void TickCameraArmLength(float DeltaSeconds);
#pragma endregion

#pragma region PlayerStance
    /** Called via input for changing the Stance of the Character **/
    void ChangeStanceInputPressed();

    void ChangeStanceInputReleased();

    void ChangeStancePressed();

    void ChangeStanceInputHold();

    UFUNCTION(BlueprintCallable, Category = PlayerStance)
    void ChangeStance(EPlayerStanceState NewStance);

    UFUNCTION(BlueprintImplementableEvent, Category = PlayerStance)
    void OnStanceChanged(EPlayerStanceState OldStance, EPlayerStanceState NewStance);
#pragma endregion

#pragma region Aiming

protected:
    void Aim();

    void StopAim();

    UFUNCTION(BlueprintImplementableEvent, Category = Aiming)
    void OnAimChanged(bool isAiming);

#pragma endregion

#pragma region ElectricTool

    /** Spawn the Throwable Object equiped by the Player **/
    void PickThrowableObject();

    /** Throw the Throwable Object spawned by the Player **/
    void ThrowObject();

#pragma endregion


#pragma region Debug

private:
    void DrawDebug();

#pragma endregion


    /*
    *
    *    --- GETTER/SETTER ---
    *
    */

public:
    UFUNCTION(BlueprintPure, Category = PlayerStance)
    FORCEINLINE EPlayerStanceState GetPlayerStance() const { return CurrentStance; }

    UFUNCTION(BlueprintPure, Category = Aiming)
    FORCEINLINE bool IsAiming() const { return bIsAiming; }

protected:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    /*
    *
    *    --- MEMBER VARIABLES ---
    *
    */


#pragma region Camera

public:
    /** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        float BaseTurnRate;

    /** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        float BaseLookUpRate;
protected:
	
    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, Category = Camera)
    float WantedArmLength;

    /*
    *    -- STANCE CAMERA MODIFICATION --
    */
    UPROPERTY(EditAnywhere, Category = Camera)
    float StanceArmLengthTransitionDuration;

    UPROPERTY(EditAnywhere, Category = Camera)
    float StandStanceArmLength;

    UPROPERTY(EditAnywhere, Category = Camera)
    float CrouchStanceArmLength;

    UPROPERTY(EditAnywhere, Category = Camera)
    float ProneStanceArmLength;

    /*
    *    -- AIM CAMERA MODIFICATION --
    */
    UPROPERTY(EditAnywhere, Category = Camera)
    float AimingArmLengthTransitionDuration;

    UPROPERTY(EditAnywhere, Category = Camera)
    float AimingArmLength;

private:
    // The Arm Length speed (in m/s) needed in order to go from Current Arm Length to Wanted Arm Length in "ArmLengthTransitionDuration" seconds
    UPROPERTY(VisibleAnywhere, Category = Camera)
    float ArmLengthSpeed;

#pragma endregion Member Variables for Camera Handling

#pragma region PlayerStance

protected:
    UPROPERTY(VisibleAnywhere, Category = PlayerStance, meta = (AllowPrivateAccess = "true"))
    EPlayerStanceState  CurrentStance;

    UPROPERTY(VisibleAnywhere, Category = PlayerStance, meta = (AllowPrivateAccess = "true"))
    bool                bIsChangeStanceInputPressed;

    UPROPERTY(VisibleAnywhere, Category = PlayerStance, meta = (AllowPrivateAccess = "true"))
    bool                bIsChangeStanceInputHolded;

    UPROPERTY(VisibleAnywhere, Category = PlayerStance, meta = (AllowPrivateAccess = "true"))
    FTimerHandle        TimeSinceStancePressed;

    /* Delay (in seconds) need for the input the change the stance when holding the button */
    UPROPERTY(EditAnywhere, Category = PlayerStance)
    float               DelayForStanceHold;

#pragma endregion Member Variables to handle the Player Stances (Stand/Crouch/Prone)

#pragma region Aiming

protected:
    UPROPERTY(VisibleAnywhere, Category = Aiming, meta = (AllowPrivateAccess = "true"))
    bool bIsAiming;

#pragma endregion

#pragma region ElectricTool

    UPROPERTY(EditAnywhere, Category = "Inventory")
    TSubclassOf<AActor> EquipedThrowableObject;

    AActor*             SpawnedThrowableObject;

#pragma endregion Member Variables for Electric Component the player can use


#pragma region Debug

private:
    UPROPERTY(EditAnywhere, Category = "Debug")
    bool                bEnableDebugDraw;
};

#pragma endregion