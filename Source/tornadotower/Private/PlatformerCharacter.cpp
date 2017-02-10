// Fill out your copyright notice in the Description page of Project Settings.

#include "tornadotower.h"
#include "GameFramework/DefaultPawn.h"
#include "../Public/PlatformerCharacter.h"
#include "../Public/PlatformerPlayerMovementComp.h"
#include "../Public/PlatformerPlayerController.h"

APlatformerCharacter::APlatformerCharacter (const FObjectInitializer& ObjectInitializer)
	: Super (ObjectInitializer.SetDefaultSubobjectClass<UPlatformerPlayerMovementComp> (ACharacter::CharacterMovementComponentName)) {
	MinSpeedForHittingWall = 200.0f;
	GetMesh ()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;

	// Set size for collision capsule
	GetCapsuleComponent ()->InitCapsuleSize (42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement ()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement ()->RotationRate = FRotator (0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement ()->JumpZVelocity = 600.f;
	GetCharacterMovement ()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent> (TEXT ("CameraBoom"));
	CameraBoom->SetupAttachment (RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent> (TEXT ("FollowCamera"));
	FollowCamera->SetupAttachment (CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void APlatformerCharacter::PostInitializeComponents () {
	Super::PostInitializeComponents ();

	// setting initial rotation
	SetActorRotation (FRotator (0.0f, 0.0f, 0.0f));
}

void APlatformerCharacter::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent) {
	PlayerInputComponent->BindAction ("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction ("Jump", IE_Released, this, &ACharacter::StopJumping);
	//PlayerInputComponent->BindAction ("Slide", IE_Pressed, this, &APlatformerCharacter::OnStartSlide);
	//PlayerInputComponent->BindAction ("Slide", IE_Released, this, &APlatformerCharacter::OnStopSlide);

	PlayerInputComponent->BindAxis ("MoveForward", this, &APlatformerCharacter::MoveForward);
	PlayerInputComponent->BindAxis ("MoveRight", this, &APlatformerCharacter::MoveRight);

	/*PlayerInputComponent->BindAxis ("MoveUp", this, &APlatformerCharacter::MoveUp_World);

	PlayerInputComponent->BindAxis ("Turn", this, &APlatformerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis ("TurnRate", this, &APlatformerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis ("LookUp", this, &APlatformerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis ("LookUpRate", this, &APlatformerCharacter::LookUpAtRate);*/
}

void APlatformerCharacter::OnStartJump () {
	UE_LOG (LogTemp, Warning, TEXT ("On Start Jumping!"));

	APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController> (Controller);
	if (MyPC) {
		if (MyPC->TryStartingGame ()) {
			return;
		}

		// if && MyGame->IsRoundInProgress ()
		if (!MyPC->IsMoveInputIgnored ()) {
			bPressedJump = true;
		}
	}
}

void APlatformerCharacter::OnStopJump () {
	UE_LOG (LogTemp, Warning, TEXT ("On STOP Jumping!"));

	bPressedJump = false;
	StopJumping ();
}

void APlatformerCharacter::MoveRight (float Value) {
	if ((Controller != NULL) && (Value != 0.0f)) {
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation ();
		const FRotator YawRotation (0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::Y);
		// add movement in that direction
		AddMovementInput (Direction, Value);
	}
}

void APlatformerCharacter::MoveForward (float Value) {
	if ((Controller != NULL) && (Value != 0.0f)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation ();
		const FRotator YawRotation (0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix (YawRotation).GetUnitAxis (EAxis::X);
		AddMovementInput (Direction, Value);
	}
}

void APlatformerCharacter::MoveUp_World (float Val) {

}

void APlatformerCharacter::TurnAtRate (float Rate) {

}

void APlatformerCharacter::LookUpAtRate (float Rate) {

}

void APlatformerCharacter::AddControllerPitchInput (float Val) {

}

void APlatformerCharacter::AddControllerYawInput (float Val) {

}

bool APlatformerCharacter::IsSliding () {
	UPlatformerPlayerMovementComp* MoveComp = Cast<UPlatformerPlayerMovementComp> (GetCharacterMovement ());
	return MoveComp && MoveComp->IsSliding ();
}

void APlatformerCharacter::CheckJumpInput (float DeltaTime) {
	if (bPressedJump) {
		UPlatformerPlayerMovementComp* MoveComp = Cast<UPlatformerPlayerMovementComp> (GetCharacterMovement ());
		if (MoveComp && MoveComp->IsSliding ()) {
			MoveComp->TryToEndSlide ();
			return;
		}
	}

	Super::CheckJumpInput (DeltaTime);
}

void APlatformerCharacter::Tick (float DeltaSeconds) {
	// decrease anim position adjustment
	if (!AnimPositionAdjustment.IsNearlyZero ()) {
		AnimPositionAdjustment = FMath::VInterpConstantTo (AnimPositionAdjustment, FVector::ZeroVector, DeltaSeconds, 400.0f);
		GetMesh ()->SetRelativeLocation (GetBaseTranslationOffset () + AnimPositionAdjustment);
	}

	if (ClimbToMarker) {
		// correction in case climb marker is moving
		const FVector AdjustDelta = ClimbToMarker->GetComponentLocation () - ClimbToMarkerLocation;
		if (!AdjustDelta.IsZero ()) {
			SetActorLocation (GetActorLocation () + AdjustDelta, false);
			ClimbToMarkerLocation += AdjustDelta;
		}
	}

	Super::Tick (DeltaSeconds);

}

void APlatformerCharacter::PlayRoundFinished () {
	//APlatformerGameMode* MyGame = GetWorld ()->GetAuthGameMode<APlatformerGameMode> ();
	//const bool bWon = MyGame && MyGame->IsRoundWon ();

	//PlayAnimMontage (bWon ? WonMontage : LostMontage);

	GetCharacterMovement ()->StopMovementImmediately ();
	GetCharacterMovement ()->DisableMovement ();
}

void APlatformerCharacter::OnRoundFinished () {
	// don't stop in mid air, will be continued from Landed() notify
	if (GetCharacterMovement ()->MovementMode != MOVE_Falling) {
		PlayRoundFinished ();
	}
}

void APlatformerCharacter::OnRoundReset () {
	// reset animations
	/*if (GetMesh () && GetMesh ()->AnimScriptInstance) {
	GetMesh ()->AnimScriptInstance->Montage_Stop (0.0f);
	}*/

	// reset movement properties
	GetCharacterMovement ()->StopMovementImmediately ();
	GetCharacterMovement ()->SetMovementMode (MOVE_Walking);
	bPressedJump = false;
	bPressedSlide = false;
}
//
//void APlatformerCharacter::Landed (const FHitResult& Hit) {
//	Landed (Hit);
//
//	//APlatformerGameMode* MyGame = GetWorld ()->GetAuthGameMode<APlatformerGameMode> ();
//	//if (MyGame && MyGame->GetGameState () == EGameState::Finished) {
//	PlayRoundFinished ();
//	//}
//}

void APlatformerCharacter::MoveBlockedBy (const FHitResult& Impact) {
	const float ForwardDot = FVector::DotProduct (Impact.Normal, FVector::ForwardVector);
	if (GetCharacterMovement ()->MovementMode != MOVE_None) {
		/*UE_LOG (LogPlatformer, Log, TEXT ("Collision with %s, normal=(%f,%f,%f), dot=%f, %s"),
		*GetNameSafe (Impact.Actor.Get ()),
		Impact.Normal.X, Impact.Normal.Y, Impact.Normal.Z,
		ForwardDot,
		*GetCharacterMovement ()->GetMovementName ());*/
	}

	if (GetCharacterMovement ()->MovementMode == MOVE_Walking && ForwardDot < -0.9f) {
		UPlatformerPlayerMovementComp* MyMovement = Cast<UPlatformerPlayerMovementComp> (GetCharacterMovement ());
		const float Speed = FMath::Abs (FVector::DotProduct (MyMovement->Velocity, FVector::ForwardVector));
		// if running or sliding: play bump reaction and jump over obstacle

		float Duration = 0.01f;
		if (Speed > MinSpeedForHittingWall) {
			Duration = PlayAnimMontage (HitWallMontage);
		}
		GetWorldTimerManager ().SetTimer (TimerHandle_ClimbOverObstacle, this, &APlatformerCharacter::ClimbOverObstacle, Duration, false);
		MyMovement->PauseMovementForObstacleHit ();
	}
	//else if (GetCharacterMovement ()->MovementMode == MOVE_Falling) {
	//	// if in mid air: try climbing to hit marker
	//	APlatformerClimbMarker* Marker = Cast<APlatformerClimbMarker> (Impact.Actor.Get ());
	//	if (Marker) {
	//		ClimbToLedge (Marker);

	//		UPlatformerPlayerMovementComp* MyMovement = Cast<UPlatformerPlayerMovementComp> (GetCharacterMovement ());
	//		MyMovement->PauseMovementForLedgeGrab ();
	//	}
	//}
}

void APlatformerCharacter::ResumeMovement () {
	SetActorEnableCollision (true);

	// restore movement state and saved speed
	UPlatformerPlayerMovementComp* MyMovement = Cast<UPlatformerPlayerMovementComp> (GetCharacterMovement ());
	MyMovement->RestoreMovement ();

	ClimbToMarker = NULL;
}

void APlatformerCharacter::ClimbOverObstacle () {
	// climbing over obstacle:
	// - there are three animations matching with three types of predefined obstacle heights
	// - pawn is moved using root motion, ending up on top of obstacle as animation ends

	const FVector ForwardDir = GetActorForwardVector ();
	const FVector TraceStart = GetActorLocation () + ForwardDir * 150.0f + FVector (0, 0, 1) * (GetCapsuleComponent ()->GetScaledCapsuleHalfHeight () + 150.0f);
	const FVector TraceEnd = TraceStart + FVector (0, 0, -1) * 500.0f;

	FCollisionQueryParams TraceParams (NAME_None, true);
	FHitResult Hit;
	GetWorld ()->LineTraceSingleByChannel (Hit, TraceStart, TraceEnd, ECC_Pawn, TraceParams);

	if (Hit.bBlockingHit) {
		const FVector DestPosition = Hit.ImpactPoint + FVector (0, 0, GetCapsuleComponent ()->GetScaledCapsuleHalfHeight ());
		const float ZDiff = DestPosition.Z - GetActorLocation ().Z;
		/*UE_LOG (LogPlatformer, Log, TEXT ("Climb over obstacle, Z difference: %f (%s)"), ZDiff,
		(ZDiff < ClimbOverMidHeight) ? TEXT ("small") : (ZDiff < ClimbOverBigHeight) ? TEXT ("mid") : TEXT ("big"));*/

		UAnimMontage* Montage = (ZDiff < ClimbOverMidHeight) ? ClimbOverSmallMontage : (ZDiff < ClimbOverBigHeight) ? ClimbOverMidMontage : ClimbOverBigMontage;

		// set flying mode since it needs Z changes. If Walking or Falling, we won't be able to apply Z changes
		// this gets reset in the ResumeMovement
		GetCharacterMovement ()->SetMovementMode (MOVE_Flying);
		SetActorEnableCollision (false);
		const float Duration = PlayAnimMontage (Montage);
		GetWorldTimerManager ().SetTimer (TimerHandle_ResumeMovement, this, &APlatformerCharacter::ResumeMovement, Duration - 0.1f, false);
	} else {
		// shouldn't happen
		ResumeMovement ();
	}
}

void APlatformerCharacter::OnStartSlide () {
	
	UE_LOG (LogTemp, Warning, TEXT ("On Start Sliding!"));
	
	APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController> (Controller);
	if (MyPC) {
		if (MyPC->TryStartingGame ()) {
			return;
		}

		// if && MyGame->IsRoundInProgress ()
		if (!MyPC->IsMoveInputIgnored ()) {
			bPressedSlide = true;
		}
	}
}

void APlatformerCharacter::PlaySlideStarted () {
	if (SlideSound) {
		SlideAC = UGameplayStatics::SpawnSoundAttached (SlideSound, GetMesh ());
	}
}

bool APlatformerCharacter::WantsToSlide () {
	return bPressedSlide;
}

float APlatformerCharacter::GetCameraHeightChangeThreshold () {
	return CameraHeightChangeThreshold;
}

void APlatformerCharacter::OnStopSlide () {
	UE_LOG (LogTemp, Warning, TEXT ("On STOP Sliding!"));

	bPressedSlide = false;
}

void APlatformerCharacter::PlaySlideFinished () {
	if (SlideAC) {
		SlideAC->Stop ();
		SlideAC = NULL;
	}
}