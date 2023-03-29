// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GrabComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PERIPHERAL_API UGrabComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void SetPrimitveComponentPhysics(bool sim);

	UFUNCTION(BlueprintCallable)
		virtual bool TryGrab(class UMotionControllerComponent* mc);

	UFUNCTION(BlueprintCallable)
		virtual bool TryRelease();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsHeld = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bSimulateOnDrop = true;

	virtual void OnGrabbed();
	virtual void OnReleased();

	UMotionControllerComponent* mMC;
};
