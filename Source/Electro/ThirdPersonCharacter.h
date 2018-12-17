// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerStanceState : uint8
{
    Stand,
    Crouch,
    Prone,
    Invalid,
};

UCLASS(config = Game)
class ELECTRO_API AThirdPersonCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AThirdPersonCharacter();

    /*
    *
    *    --- FUNCTIONS ---
    *
    */

protected:
    virtual void Tick(float DeltaSeconds) override;

    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // End of APawn interface

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

#pragma endregion

#pragma region ElectricTool

    /** Spawn the Throwable Object equiped by the Player **/
    void PickThrowableObject();

    /** Throw the Throwable Object spawned by the Player **/
    void ThrowObject();

#pragma endregion

#pragma region PlayerStance
    /** Called via input for changing the Stance of the Character **/
    void ChangeStanceInputPressed();

    void ChangeStanceInputReleased();

    void ChangeStancePressed();

    void ChangeStanceInputHold();

    void ChangeStance(EPlayerStanceState OldStance, EPlayerStanceState NewStance);
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

#pragma endregion Member Variables for Camera Handling

#pragma region PlayerStance

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerControl, meta = (AllowPrivateAccess = "true"))
        EPlayerStanceState  CurrentStance;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerControl, meta = (AllowPrivateAccess = "true"))
        bool                bIsChangeStanceInputPressed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerControl, meta = (AllowPrivateAccess = "true"))
        bool                bIsChangeStanceInputHolded;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerControl, meta = (AllowPrivateAccess = "true"))
        FTimerHandle        TimeSinceStancePressed;

    /* Delay (in seconds) need for the input the change the stance when holding the button */
    UPROPERTY(EditAnywhere, Category = PlayerControl)
        float               DelayForStanceHold;

#pragma endregion Member Variables to handle the Player Stances (Stand/Crouch/Prone)

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