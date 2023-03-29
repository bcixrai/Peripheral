// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabComponent.h"
#include "MotionControllerComponent.h"

// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGrabComponent::SetPrimitveComponentPhysics(bool sim)
{
	auto parent = GetAttachParent();
	if (!parent) {
		return;
	}
	auto prim = Cast<UPrimitiveComponent>(parent);
	if (!prim) {
		return;
	}
	prim->SetSimulatePhysics(sim);
}

bool UGrabComponent::TryGrab(UMotionControllerComponent* mc)
{
	SetPrimitveComponentPhysics(false);;
	bool attach = GetAttachParent()->AttachToComponent(mc, FAttachmentTransformRules::KeepWorldTransform);

	if (attach) {
		bIsHeld = true;
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Grabbed")));

		mMC = mc;
		OnGrabbed();


		return true;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Not Grabbed")));

		return false;
	}
}

bool UGrabComponent::TryRelease()
{
	if (bSimulateOnDrop) {
		SetPrimitveComponentPhysics(true);
	}

	auto parent = GetAttachParent();
	if (!parent) {

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Not released")));
		return false;
	}
	parent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	bIsHeld = false;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Released")));
	mMC = nullptr;
	OnReleased();
	return true;
}

void UGrabComponent::OnGrabbed()
{
}

void UGrabComponent::OnReleased()
{
}

