// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalInteractionComponent.h"
#include "Animal.h"
#include "VRPlayer.h"
// Sets default values for this component's properties
UAnimalInteractionComponent::UAnimalInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAnimalInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	auto animal = Cast<AAnimal>(GetOwner());
	if (animal) {
		mAnimal = animal;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Animal IC : No Animal")));
	}
}


// Called every frame
void UAnimalInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UAnimalInteractionComponent::Interact(AActor* interactor)
{
	auto player = Cast<AVRPlayer>(interactor);
	if (!player) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Animal IC : Not Player")));
		return false;
	}
	if (!mAnimal) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Animal IC : No Animal")));
		return false;
	}
	mAnimal->InteractWithPlayer(this, mReactionType);
	return true;
}

