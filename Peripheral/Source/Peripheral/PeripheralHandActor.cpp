// Fill out your copyright notice in the Description page of Project Settings.


#include "PeripheralHandActor.h"
#include "MotionControllerComponent.h"
// Sets default values
APeripheralHandActor::APeripheralHandActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APeripheralHandActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APeripheralHandActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APeripheralHandActor::AttachToMotionController(UMotionControllerComponent* mc)
{
	//Are we already attached to something ? 
	if (bAttatched) {
		return false;
	}
	AttachToComponent(mc, FAttachmentTransformRules::SnapToTargetIncludingScale);
	bAttatched = true;
	//Check for attachment
	if (!IsAttachedTo(mc->GetOwner())) {
		bAttatched = false;
	}
	return bAttatched;
}

bool APeripheralHandActor::AttachToMotionController()
{
	if (mMotionController) {
		return AttachToMotionController(mMotionController);
	}
	return false;
}

bool APeripheralHandActor::DetachFromMotionController(UMotionControllerComponent* mc)
{
	//Check we're actualyl attached to the motion controller
	if (!IsAttachedTo(mc->GetOwner())) {
		//Return false if we're not attached
		return false;
	}
	//Deat
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//We should now be detatched
	bAttatched = false;
	//Check detatchment
	if (IsAttachedTo(mc->GetOwner())) {
		//Something is wrong
		bAttatched = true;
	}
	
	//If bAttached is false, we want to return true
	return !bAttatched;
}

bool APeripheralHandActor::DetachFromMotionController()
{
	if (mMotionController) {
		return DetachFromMotionController(mMotionController);
	}
	return false;
}

void APeripheralHandActor::SetMotionController(UMotionControllerComponent* mc)
{
	//Are we already de
	mMotionController = mc;
}

