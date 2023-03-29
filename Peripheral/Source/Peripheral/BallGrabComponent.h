// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabComponent.h"
#include "BallGrabComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PERIPHERAL_API UBallGrabComponent : public UGrabComponent
{
	GENERATED_BODY()
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnGrabbed()  override;
	virtual void OnReleased() override;

	bool bCheckForThrow = false;

	float mCurrentTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mCheckForThrowTime = 0.1f;

	//How fast should it move to be a throw, gravity will move it 9.8 ish meter per seconds (without air and such)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mRequiredMetersPerSecond = 2.f;

	//Where was the ball released from
	FVector mReleasedLocation = FVector{0,0,0};

};
