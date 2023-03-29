// Copyright Epic Games, Inc. All Rights Reserved.


#include "PeripheralGameModeBase.h"

APeripheralGameModeBase::APeripheralGameModeBase()
{
	//auto per = Cast<UPeripheralGameInstance>(GetGameInstance());
	//if (per) {
	//	mPeripheralGI = per;
	//}

}

void APeripheralGameModeBase::StartPlay()
{
	Super::StartPlay();
	auto per = Cast<UPeripheralGameInstance>(GetGameInstance());
	if (!per) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("GM : No game Instance")));
		return;
	}
	mPeripheralGI = per;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("GM : Set GameInstance")));
	
}
