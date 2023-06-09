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
	auto parent = GetOwner();
	if (!parent) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("SetPrimitivePhysics : No Parent")));
		return;
	}
	auto prim = Cast<UPrimitiveComponent>(parent);
	
	prim = Cast<UPrimitiveComponent>(parent->GetComponentByClass(UPrimitiveComponent::StaticClass()));
	if (!prim) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("SetPrimitivePhysics : No Primitive Component")));
		return;
	}
	prim->SetSimulatePhysics(sim);
	parent->SetActorEnableCollision(sim);
}

bool UGrabComponent::TryGrab(USceneComponent* mc)
{
	SetPrimitveComponentPhysics(false);;
	GetOwner()->AttachToComponent(mc, FAttachmentTransformRules::KeepWorldTransform);
	bIsHeld = true;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Grabbed")));

	mOwner = mc;
	OnGrabbed();


	return true;

}

bool UGrabComponent::TryRelease()
{
	if (bSimulateOnDrop) {
		SetPrimitveComponentPhysics(true);
	}

	auto parent = GetOwner();
	if (!parent) {

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Not released")));
		return false;
	}

	GetOwner()->DetachRootComponentFromParent();
	bIsHeld = false;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Released")));
	mOwner = nullptr;
	OnReleased();
	return true;
}

bool UGrabComponent::ForceGrab(USceneComponent* owner, bool handleAttachment)
{
	if (mOwner && mOwner != owner) {
		return false; //Is already grabbed
	}
	mOwner = owner;
	if (!handleAttachment) {
		return true;
	}
	SetPrimitveComponentPhysics(false);
	//Handle attachment
	GetOwner()->AttachToComponent(mOwner, FAttachmentTransformRules::KeepWorldTransform);
	
	OnGrabbed();
	return true;
}

bool UGrabComponent::ForceReleased(USceneComponent* owner, bool handleDeattachment)
{
	if (owner != mOwner) {
		return false;
	}
	mOwner = nullptr;
	if (!handleDeattachment) {
		return true;
	}
	GetOwner()->DetachRootComponentFromParent();
	if (bSimulateOnDrop) {
		SetPrimitveComponentPhysics(true);
	}

	OnReleased();
	return false;
}

void UGrabComponent::OnGrabbed()
{
}

void UGrabComponent::OnReleased()
{
}

//bool UGrabComponent::Interact(AActor* interactor)
//{
//	return false;
//}

