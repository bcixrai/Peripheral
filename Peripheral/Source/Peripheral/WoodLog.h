// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "WoodLog.generated.h"

UCLASS()
class PERIPHERAL_API AWoodLog : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWoodLog();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* mMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UGrabComponent* mGrab;


	virtual bool Interact(AActor* interactor) override;
	virtual FVector GetInteractableLocation() override {
		return GetActorLocation();
	};
};
