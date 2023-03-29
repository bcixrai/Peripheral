// Fill out your copyright notice in the Description page of Project Settings.


#include "HandleComponent.h"

#include "MotionControllerComponent.h"
void UHandleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bIsHeld && mMC) {
		//The handle is held by a MotionController

		
		//What axis are we rotating on ?
		FVector mcLoc = mMC->GetComponentLocation();
		float rotValue = 0;
		if (mRotationAxis == X) {
			//
			mcLoc.X = GetComponentLocation().X;
			rotValue = GetComponentRotation().Roll;
		}
		if (mRotationAxis == Y) {
			//
			mcLoc.Y = GetComponentLocation().Y;
			rotValue = GetComponentRotation().Pitch;
		}
		if (mRotationAxis == Z) {
			//
			mcLoc.Z = GetComponentLocation().Z;
			rotValue = GetComponentRotation().Yaw;
		}

		//Delta rotation value
		float deltaRot = rotValue - mLastRotValue;

		//The mc is now at the same X axis as the handle origin
		mcLoc;
		//We need some 

		//The X 
		mLastRotValue = rotValue;

		//Capture new angle between handle and mc
	}
}

bool UHandleComponent::TryGrab(UMotionControllerComponent* mc)
{
	//What happens when the handle is grabbed ? 
	if (bIsHeld) {
		//its already held so return false
		return false;
	}

	//IF its not already held, then we're now held
	mMC = mc;
	bIsHeld = true;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Handle : Grabbed")));

	//Capture angle between the mc and the handle
	//What axis are we rotating on ?
	FVector mcLoc = mMC->GetComponentLocation();
	float rotValue = 0;
	if (mRotationAxis == X) {
		//
		mcLoc.X = GetComponentLocation().X;
		rotValue = GetComponentRotation().Roll;
	}
	if (mRotationAxis == Y) {
		//
		mcLoc.Y = GetComponentLocation().Y;
		rotValue = GetComponentRotation().Pitch;
	}
	if (mRotationAxis == Z) {
		//
		mcLoc.Z = GetComponentLocation().Z;
		rotValue = GetComponentRotation().Yaw;
	}

	//If we cross it with the forward ? 
	FVector myloc = FVector(0, 0, 0);
	FVector offset = GetComponentLocation() - mcLoc;

	//OFfset should now be something ( 0, 14, 38)
	// do we normalize it ? 
	offset.Normalize();
	//We now have a vector from the origin of the handle out toward


	return true;
}

bool UHandleComponent::TryRelease()
{
	//What happens when we try to release it ? 
	//We always just release it
	bIsHeld = false;
	mMC = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Handle : Released")));
	


	return true;

}

void UHandleComponent::Turn()
{
	auto rot = GetComponentRotation();
	rot.Roll += GetWorld()->GetDeltaSeconds() * mTurnSpeed;

	SetWorldRotation(rot);
}
