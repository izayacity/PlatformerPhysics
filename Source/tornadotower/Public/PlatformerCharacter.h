// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "tornadotower.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "PlatformerCharacter.generated.h"

// ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)
UCLASS()
class TORNADOTOWER_API APlatformerCharacter : public ACharacter {
	GENERATED_UCLASS_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** player pawn initialization */
	virtual void PostInitializeComponents ();

	/** perform position adjustments */
	virtual void Tick (float DeltaSeconds);

	/** setup input bindings */
	virtual void SetupPlayerInputComponent (class UInputComponent* InputComponent);

	/** used to make pawn jump ; overridden to handle additional jump input functionality */
	virtual void CheckJumpInput (float DeltaTime);

	/** notify from movement about hitting an obstacle while running */
	virtual void MoveBlockedBy (const FHitResult& Impact);

	/** play end of round if game has finished with character in mid air */
	//virtual void Landed (const FHitResult& Hit);

	/** try playing end of round animation */
	void OnRoundFinished ();

	/** stop any active animations, reset movement state */
	void OnRoundReset ();

	/** returns true when pawn is sliding ; used in AnimBlueprint */
	UFUNCTION(BlueprintCallable, Category = "Pawn|Character")
	bool IsSliding ();

	/** gets bPressedSlide value */
	bool WantsToSlide ();

	/** event called when player presses jump button */
	void OnStartJump ();

	/** event called when player releases jump button */
	void OnStopJump ();

	/** event called when player presses slide button */
	void OnStartSlide ();

	/** event called when player releases slide button */
	void OnStopSlide ();

	/** handle effects when slide starts */
	void PlaySlideStarted ();

	/** handle effects when slide is finished */
	void PlaySlideFinished ();

	/** gets CameraHeightChangeThreshold value */
	float GetCameraHeightChangeThreshold ();


	/**
	* Input callback to move forward in local space (or backward if Val is negative).
	* @param Val Amount of movement in the forward direction (or backward if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION (BlueprintCallable, Category = "PlatformerCharacter | Input")
		virtual void MoveForward (float Val);

	/**
	* Input callback to strafe right in local space (or left if Val is negative).
	* @param Val Amount of movement in the right direction (or left if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION (BlueprintCallable, Category = "PlatformerCharacter | Input")
		virtual void MoveRight (float Val);

	/**
	* Input callback to move up in world space (or down if Val is negative).
	* @param Val Amount of movement in the world up direction (or down if negative).
	* @see APawn::AddMovementInput()
	*/
	UFUNCTION (BlueprintCallable, Category = "PlatformerCharacter | Input")
		virtual void MoveUp_World (float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	UFUNCTION (BlueprintCallable, Category = "PlatformerCharacter | Input")
		void TurnAtRate (float Rate);

	/**
	* Called via input to look up at a given rate (or down if Rate is negative).
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	UFUNCTION (BlueprintCallable, Category = "PlatformerCharacter | Input")
		void LookUpAtRate (float Rate);

	/**
	* Add input (affecting Pitch) to the Controller's ControlRotation, if it is a local PlayerController.
	* This value is multiplied by the PlayerController's InputPitchScale value.
	* @param Val Amount to add to Pitch. This value is multiplied by the PlayerController's InputPitchScale value.
	* @see PlayerController::InputPitchScale
	*/
	UFUNCTION (BlueprintCallable, Category = "PlatformerCharacter", meta = (Keywords = "up down addpitch"))
		virtual void AddControllerPitchInput (float Val);

	/**
	* Add input (affecting Yaw) to the Controller's ControlRotation, if it is a local PlayerController.
	* This value is multiplied by the PlayerController's InputYawScale value.
	* @param Val Amount to add to Yaw. This value is multiplied by the PlayerController's InputYawScale value.
	* @see PlayerController::InputYawScale
	*/
	UFUNCTION (BlueprintCallable, Category = "PlatformerCharacter", meta = (Keywords = "left right turn addyaw"))
		virtual void AddControllerYawInput (float Val);

private:
	/**
	* Camera is fixed to the ground, even when player jumps.
	* But, if player jumps higher than this threshold, camera will start to follow.
	*/
	UPROPERTY (EditDefaultsOnly, Category = Config)
		float CameraHeightChangeThreshold;

	/** animation for winning game */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		UAnimMontage* WonMontage;

	/** animation for loosing game */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		UAnimMontage* LostMontage;

	/** animation for running into an obstacle */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		UAnimMontage* HitWallMontage;

	/** minimal speed for pawn to play hit wall animation */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		float MinSpeedForHittingWall;

	/** animation for climbing over small obstacle */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		UAnimMontage* ClimbOverSmallMontage;

	/** height of small obstacle */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		float ClimbOverSmallHeight;

	/** animation for climbing over mid obstacle */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		UAnimMontage* ClimbOverMidMontage;

	/** height of mid obstacle */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		float ClimbOverMidHeight;

	/** animation for climbing over big obstacle */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		UAnimMontage* ClimbOverBigMontage;

	/** height of big obstacle */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		float ClimbOverBigHeight;

	/** animation for climbing to ledge */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		UAnimMontage* ClimbLedgeMontage;

	/** root offset in climb legde animation */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		FVector ClimbLedgeRootOffset;

	/** grab point offset along X axis in climb legde animation */
	UPROPERTY (EditDefaultsOnly, Category = Animation)
		float ClimbLedgeGrabOffsetX;

	/** mesh translation used for position adjustments */
	FVector AnimPositionAdjustment;

	/** root motion translation from previous tick */
	FVector PrevRootMotionPosition;

	/** looped slide sound */
	UPROPERTY (EditDefaultsOnly, Category = Sound)
		USoundCue* SlideSound;

	/** audio component playing looped slide sound */
	UPROPERTY ()
		UAudioComponent* SlideAC;

	/** true when player is holding slide button */
	uint32 bPressedSlide : 1;

	/** true when player is holding jump button */
	uint32 bPressedJump : 1;

	/** ClimbMarker (or to be exact its mesh component - the movable part) we are climbing to */
	UPROPERTY ()
		UStaticMeshComponent* ClimbToMarker;

	/** location of ClimbMarker we are climbing to */
	FVector ClimbToMarkerLocation;

	/** Handle for efficient management of ClimbOverObstacle timer */
	FTimerHandle TimerHandle_ClimbOverObstacle;

	/** Handle for efficient management of ResumeMovement timer */
	FTimerHandle TimerHandle_ResumeMovement;

	/** determine obstacle height type and play animation */
	void ClimbOverObstacle ();

	/** restore pawn's movement state */
	void ResumeMovement ();

	/** play end of round animation */
	void PlayRoundFinished ();
};
