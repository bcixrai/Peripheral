// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimalAnimationInstance.h"
#include "Animal.h"
void UAnimalAnimationInstance::NativeInitializeAnimation()
{
	mAnimal = Cast<AAnimal>(TryGetPawnOwner());
	if (!mAnimal) {
		//Something very wrong
	}
}

void UAnimalAnimationInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!mAnimal) {
		return;
	}

	bTaunting = mAnimal->GetMode() == TAUNT;

	
	//IF we're not taunting, dont do anything more
	if (bTaunting) {
		mTauntType = mAnimal->GetCurrentTauntType();
		return;
	}

	mSpeed = mAnimal->GetCurrentSpeed();

	
}
