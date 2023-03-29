// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "MusicBox.generated.h"

UCLASS()
class PERIPHERAL_API AMusicBox : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UHandleComponent* mHandle;

	virtual bool Interact(AActor* interactor) override;
	virtual FVector GetInteractableLocation() override {
		return GetActorLocation();
	};


};
