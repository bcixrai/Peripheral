// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PeripheralHandActor.generated.h"

UCLASS()
class PERIPHERAL_API APeripheralHandActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APeripheralHandActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Attach to motion controller
	bool AttachToMotionController(class UMotionControllerComponent* mc);
	bool AttachToMotionController();
	//Detach from motion controller
	bool DetachFromMotionController(UMotionControllerComponent* mc);
	bool DetachFromMotionController();
	//Set motion controller
	void SetMotionController(UMotionControllerComponent* mc);
private:
	//A refrence to a motion controller
	UMotionControllerComponent* mMotionController;

	UMotionControllerComponent* mParent;
	bool bAttatched = false;
};
