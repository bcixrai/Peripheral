// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PeripheralGameInstance.h"
#include "PeripheralGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PERIPHERAL_API APeripheralGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	APeripheralGameModeBase();

	virtual void StartPlay() override;
private:
	
	//Member
	UPeripheralGameInstance* mPeripheralGI;
};
