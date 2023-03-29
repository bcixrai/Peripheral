// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"
#include "GrabComponent.h"
#include "VRPlayer.h"
// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//mGrab = CreateDefaultSubobject<UGrabComponent>(TEXT("Grab Component"));
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	
	auto grab = Cast<UGrabComponent>(GetComponentByClass(UGrabComponent::StaticClass()));
	if (grab) {
		mGrab = grab;
	}
	auto mesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (mesh) {
		mMesh = mesh;
	}
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABall::PickedUp()
{
	mMesh->SetSimulatePhysics(false);
}

void ABall::Dropped()
{

	mMesh->SetSimulatePhysics(true);
}

