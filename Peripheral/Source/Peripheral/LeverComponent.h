// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GrabComponent.h"
#include "Components/SceneComponent.h"
#include "LeverComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PERIPHERAL_API ULeverComponent : public UGrabComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULeverComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
		void HandleLever(float deltaTime);
	UFUNCTION(BlueprintCallable)
		float GetLevelPercentage();
	UFUNCTION(BlueprintCallable)
		void ResetLever();
		bool TryGrab(class UMotionControllerComponent* mc);
		bool TryRelease();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsPulled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bResetOnDrop = false;

	//What axis can the lever move
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector mLocalMovementAxis = { 0, 1,  0};


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mAngle = 0; //10 cm per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mMinAngle = -90.f; //10 cm per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mMaxAngle = 90.f; //10 cm per second


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* mHandle;

	FRotator mHandleZeroedRotation;
	
private:
	UMotionControllerComponent* mHand;
};
