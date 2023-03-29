// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabComponent.h"
#include "Interactable.h"
#include "HandleComponent.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EHandleRotationAxis{ X, Y, Z};
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PERIPHERAL_API UHandleComponent : public UGrabComponent, public IInteractable
{
	GENERATED_BODY()
public:
		// Called every frame
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		virtual bool TryGrab(class UMotionControllerComponent* mc) override;

		virtual bool TryRelease() override;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			TEnumAsByte<EHandleRotationAxis> mRotationAxis = X;

		float mLastRotValue = 0;

		void Turn();
		float mTurnSpeed = 150.f;
		virtual bool Interact(AActor* interactor)override {
			GEngine->AddOnScreenDebugMessage(52, -5, FColor::Cyan, FString::Printf(TEXT("Handle : Interaction")));
			return true;
		};
		virtual FVector GetInteractableLocation()override {
			return GetComponentLocation();
		};
};
