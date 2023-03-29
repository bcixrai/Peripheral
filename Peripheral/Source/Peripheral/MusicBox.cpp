// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicBox.h"
#include "HandleComponent.h"
// Sets default values
AMusicBox::AMusicBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	mHandle = CreateDefaultSubobject<UHandleComponent>(TEXT("Handle"));
	mHandle->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMusicBox::BeginPlay()
{
	Super::BeginPlay();
	//mHandle->SetRelativeLocation(FVector(800, -190, 210));
}

// Called every frame
void AMusicBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AMusicBox::Interact(AActor* interactor)
{
	if (mHandle) {
		mHandle->Turn();
		return true;
	}
	else {
		return false;
	}

};

