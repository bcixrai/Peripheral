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
enum EPeripheralMode { NORMAL, VR };
static EPeripheralMode PeripheralMode = NORMAL;
static EPeripheralMode GetPeripheralMode() {
	return PeripheralMode;
}
static void SetPeripheralMode(EPeripheralMode mode) {
	PeripheralMode = mode;
}
UENUM(BlueprintType)
enum EBCIMode { NO_BCI, BCI };
static EBCIMode BCIMode = NO_BCI;
static EBCIMode GetBCIMode() {
	return BCIMode;
}
static void SetBCIMode(EBCIMode mode) {
	BCIMode = mode;
}
//Debuging
UENUM(BlueprintType)
enum EDebugMode { DEBUG, NO_DEBUG };
static EDebugMode DebugMode = DEBUG;
static bool Debug() {
	return DebugMode == DEBUG;
}