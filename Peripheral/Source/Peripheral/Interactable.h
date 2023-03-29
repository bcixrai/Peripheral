// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interactable.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
	
};

class IInteractable {
	GENERATED_BODY()
public:

	virtual bool Interact(AActor* interactor)=0;
	virtual FVector GetInteractableLocation() = 0;
};
