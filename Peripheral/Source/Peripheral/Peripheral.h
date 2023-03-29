// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum EAnimalMode { WANDER, PLAYER, IDLE, BALL, TAUNT };
UENUM(BlueprintType)
enum EBallMode { NOBALL, CATCH, DELIVER, HASBALL };
UENUM(BlueprintType)
enum EAnimalInteractionType { HEAD, BELLY, BUTT, HAPPY, SAD };

//Modes
UENUM(BlueprintType)
enum EPeripheralMode { VR, BCI, VR_BCI, NORMAL };
static EPeripheralMode PeripheralMode = NORMAL;
static EPeripheralMode GetPeripheralMode() {
	return PeripheralMode;
}
static void SetPeripheralMode(EPeripheralMode mode) {
	PeripheralMode = mode;
}

//Debuging
UENUM(BlueprintType)
enum EDebugMode { DEBUG, NODEBUG };
static EDebugMode DebugMode = DEBUG;
static bool Debug() {
	return DebugMode == DEBUG;
}