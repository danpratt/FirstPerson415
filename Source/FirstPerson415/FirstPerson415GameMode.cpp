// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPerson415GameMode.h"
#include "FirstPerson415Character.h"
#include "UObject/ConstructorHelpers.h"

AFirstPerson415GameMode::AFirstPerson415GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
