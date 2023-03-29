// Fill out your copyright notice in the Description page of Project Settings.


#include "LeverComponent.h"

#include "MotionControllerComponent.h"
#include "Kismet/KismetMathLibrary.h"
// Sets default values for this component's properties
ULeverComponent::ULeverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULeverComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	auto comps = GetOwner()->GetComponentsByClass(USceneComponent::StaticClass());
	for (auto comp : comps) {
		if (comp->GetName() == "Handle") {
			mHandle = Cast<USceneComponent>(comp);
		}
	}
	if (!mHandle) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Lever did not find handle")));
	}

	mHandleZeroedRotation = mHandle->GetComponentTransform().GetRotation().Rotator();
}


// Called every frame
void ULeverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	HandleLever(DeltaTime);
	
}

void ULeverComponent::HandleLever(float deltaTime)
{
	//Do logic to be pulled
	if (!mHand) {
		return;
	}
	if (!bIsPulled) {
		return;
	}
	//
	////Hand position, the hand is a child of its owner, get relative lcoation + that actor location
	FVector handLoc = mHand->GetComponentTransform().GetLocation();
	FVector center = Cast<USceneComponent>(mHandle)->GetComponentTransform().GetLocation();
	FVector dir = center - handLoc;
	////If we're rotating along the y Axis we only want to account for the y and z axis and set x to be irrelevant
	//handLoc.X = center.X;
	//
	////float angle = FMath::Acos(FVector::CrossProduct(handLoc, center) / (handLoc.Size() * center.Size()));
	//float angle = 1;
	//mAngle = FMath::Clamp(angle, mMinAngle, mMaxAngle);
	//
	//float AngleCosine = FVector::DotProduct(mHandle->GetComponentTransform().GetLocation(), m) / (A.Size() *
	//	B.Size());
	//float AngleRadians = FMath::Acos(AngleCosine);
	//auto rot = mHandle->GetComponentTransform().GetRotation();
	//rot.X += FMath::RadiansToDegrees(acos(mHandle->GetComponentTransform().GetLocation().X / dir.Size())) * mLocalMovementAxis.X;
	//rot.Y += FMath::RadiansToDegrees(acos(mHandle->GetComponentTransform().GetLocation().Y / dir.Size())) * mLocalMovementAxis.Y;
	//rot.Z += FMath::RadiansToDegrees(acos(mHandle->GetComponentTransform().GetLocation().Z / dir.Size())) * mLocalMovementAxis.Z;
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, FString::Printf(TEXT("Pulled")));
	//
	//mHandle->SetWorldRotation(rot);
	dir *= mLocalMovementAxis;
	mHandle->SetWorldRotation(FRotationMatrix::MakeFromXZ(dir, mHandle->GetUpVector()).ToQuat());
}
float ULeverComponent::GetLevelPercentage()
{
	return 0.0f;
}

void ULeverComponent::ResetLever()
{
	//Reset to starting position
	mHandle->SetWorldRotation(mHandleZeroedRotation.Quaternion());
}

bool ULeverComponent::TryGrab(UMotionControllerComponent* mc)
{
	mHand = mc;
	bIsPulled = true;
	//Set some rotation offset
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Lever grabbed")));
	//auto rot = mHandle->GetComponentTransform().GetRotation();
	////Vector between the lever and handle
	//FVector offset = mc->GetComponentLocation() - mHandle->GetComponentTransform().GetLocation();
	//
	//auto rot = FRotationMatrix::MakeFromXZ(mHandle->GetComponentTransform().GetLocation(), mHandle->GetForwardVector()).ToQuat();
	//
	//mHandle->SetWorldRotation(rot);
	return true;
}

bool ULeverComponent::TryRelease()
{
	mHand = nullptr;
	bIsPulled = false;
	if (bResetOnDrop) {
		ResetLever();
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Lever released")));

	return true;
}

