// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Portal.generated.h"

UCLASS()
class PERIPHERAL_API APortal : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName mMapToLoadName = "M_Lobby";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* mMaterialToApply;


	virtual bool Interact(AActor* interactor) override;
	virtual FVector GetInteractableLocation() override;
};
