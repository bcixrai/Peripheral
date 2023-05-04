// Fill out your copyright notice in the Description page of Project Settings.


#include "HandleComponent.h"

#include "MotionControllerComponent.h"

void UHandleComponent::BeginPlay()
{
	Super::BeginPlay();

	auto comps = GetOwner()->GetComponents();
	for (auto comp : comps) {
		if (comp->GetName() == "Handle") {
			mHandle = Cast<USceneComponent>(comp);
		}
	}

}
void UHandleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	mLastRotValue = GetRelativeRotation().Roll;
	if (bIsHeld && mOwner) {
		//The handle is held by a MotionController
		auto base = mHandle->GetComponentLocation();
		auto pos = mOwner->GetComponentLocation();

		FVector a = FVector(0,1, 0);
		FVector b = pos - base;
		b.X = 0;
		b.Normalize();
		auto dot = FVector::DotProduct(FVector(0,0,1), b);
		auto sign = dot >= 0 ? 1 : -1;
		auto theta = FMath::Acos(FVector::DotProduct(a, b) / (a.Length() * b.Length()));
		auto rot = mHandle->GetRelativeRotation();
		auto delta = rot.Roll - (FMath::RadiansToDegrees(theta) * sign);
		rot.Roll = (FMath::RadiansToDegrees(theta) * sign);
		mHandle->SetRelativeRotation(rot);

		GEngine->AddOnScreenDebugMessage(393, 5, FColor::Cyan, FString::Printf(TEXT("Delta : %f"), delta));
		GEngine->AddOnScreenDebugMessage(392, 5, FColor::Cyan, FString::Printf(TEXT("Dot : %f"), dot));
		GEngine->AddOnScreenDebugMessage(398, 5, FColor::Cyan, FString::Printf(TEXT("Theta : %f"), FMath::RadiansToDegrees(theta)));
	}
	
}

bool UHandleComponent::TryGrab(USceneComponent* mc)
{
	//What happens when the handle is grabbed ? 
	if (bIsHeld) {
		//its already held so return false
		return false;
	}

	//IF its not already held, then we're now held
	mOwner = mc;
	bIsHeld = true;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Handle : Grabbed")));

	//Capture angle between the mc and the handle
	//What axis are we rotating on ?
	FVector mcLoc = mOwner->GetComponentLocation();
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
	mOwner = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Handle : Released")));
	


	return true;

}

bool UHandleComponent::ForceGrab(USceneComponent* owner, bool handleAttachment)
{
	if (bIsHeld) {
		//its already held so return false
		return false;
	}

	//IF its not already held, then we're now held
	mOwner = owner;
	bIsHeld = true;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Handle : Grabbed")));
	return true;
}

bool UHandleComponent::ForceReleased(USceneComponent* owner, bool handleDeattachment)
{
	//What happens when we try to release it ? 
	//We always just release it
	bIsHeld = false;
	mOwner = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Handle : Released")));



	return true;
}

void UHandleComponent::Turn()
{
	auto rot = GetComponentRotation();
	rot.Roll += GetWorld()->GetDeltaSeconds() * mTurnSpeed;

	SetWorldRotation(rot);
}
