// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayerController.h"
#include "VRPlayer.h"
#include "PeripheralGameInstance.h"
//void AVRPlayerController::OnPossess(APawn* pawn)
//{
//	Super::OnPossess(pawn);
//
//	mPlayer = Cast<AVRPlayer>(pawn);
//	if (!mPlayer) {
//
//		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("PC : Not a AVRPlayer"));
//		return;
//	}
//	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("VRPlayer Possesed"));
//}

void AVRPlayerController::BeginPlay()
{
	
	Super::BeginPlay();

	mPeripheralGI = Cast<UPeripheralGameInstance>(GetGameInstance());
	if (!mPeripheralGI) {
		//Big problem, but should never happen
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("PC : Missing Game Instance")));
	}
	mVRPlayer = Cast<AVRPlayer>(GetPawn());
	if (!mVRPlayer) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("PC : VRPlayer not found")));

	}
}

void AVRPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
