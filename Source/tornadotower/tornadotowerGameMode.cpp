// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "tornadotower.h"
#include "tornadotowerGameMode.h"
#include "tornadotowerCharacter.h"

AtornadotowerGameMode::AtornadotowerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
