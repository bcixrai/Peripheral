// Fill out your copyright notice in the Description page of Project Settings.


#include "WoodLog.h"
#include "GrabComponent.h"
// Sets default values
AWoodLog::AWoodLog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wood Mesh"));
	mGrab = CreateDefaultSubobject<UGrabComponent>(TEXT("Wood Grab"));
}

// Called when the game starts or when spawned
void AWoodLog::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWoodLog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AWoodLog::Interact(AActor* interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Wood : Interact")));
	return true;
}

