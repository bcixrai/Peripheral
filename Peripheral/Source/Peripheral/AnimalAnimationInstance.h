// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Peripheral.h"
#include "AnimalAnimationInstance.generated.h"

/**
 * 
 */


UCLASS()
class PERIPHERAL_API UAnimalAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// the below functions are the native overrides for each phase
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;
	// Native update override point. It is usually a good idea to simply gather data in this step and 
	// for the bulk of the work to be done in NativeThreadSafeUpdateAnimation.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AAnimal* mAnimal;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float mSpeed = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bTaunting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TEnumAsByte<EAnimalInteractionType> mTauntType = HAPPY;


};
