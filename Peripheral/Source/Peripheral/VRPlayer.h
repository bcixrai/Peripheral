// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Peripheral.h"
#include "VRPlayer.generated.h"

UENUM()
enum EHandSide{ right, left};
UENUM()
enum EBCIHandMovementMode { On_Input, On_Update};
class UCameraComponent;
UCLASS()
class PERIPHERAL_API AVRPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	//This VRPlayer should be able to be used for both VR and non-VR
	AVRPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class UPeripheralGameInstance* mPeripheralGI;

	//Root 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* mRoot;

	//Do general debugging
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug = true;
	void Debug();

#pragma region Camera
	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* mCamera;
	void SetCameraMode(enum EPeripheralMode mode);
	//Should the FPS Mode Movement fly around ? 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlyingCamera = true;
#pragma endregion

#pragma region Normal Movement
	//FPS MOVEment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float mMoveSpeed = 100.f;					  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float mTurnRate = 45.f;					  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float mLookUpRate = 45.f;

	void MoveForward(float value);
	void MoveRight(float value);

	void Turn(float value);
	void LookUp(float value);
#pragma endregion

	//Hands
	class APeripheralHandActor* mRightHand;
	APeripheralHandActor* mLeftHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<APeripheralHandActor> mHandActorBP;

	//This enum determines wheter the right or left hand is the one controlled by the brain
	//EHandSide mBCIHandSide = right;
	////Based on the enum, a pointer
	//TMap<TEnumAsByte<EHandSide>, APeripheralHandActor*> mHands;

	TMap<EHandSide, APeripheralHandActor*> mHandsMap;
	std::vector< APeripheralHandActor*> mHandsVector;
	TArray<APeripheralHandActor*> mHandsArray;

#pragma region VR Motion Controllers
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* mVROrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMotionControllerComponent* mRightMC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMotionControllerComponent* mLeftMC;
#pragma endregion

#pragma region BCI
	//Configure play for specific mode
	void ConfigureBCIMode(EBCIMode mode);
	//Swap global bci mode
	void SwapGlobalBCIMode();
	//Disable or enable input to the BCI hand
	void SwapBCIInputEnabled();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EHandSide> mBCIOverridenHandSide = right;
	bool bDisableVRMCInBCIMode = false;

	//BCI HAND
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BCI")
	class UBCIHandComponent* mBCIHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BCI")
		float mBCIHandMoveSpeed = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BCI")
		bool bUseInputStrength = true;
	//Moving BCI Hand
	void MoveBCIHand(FVector dir, float strength);
	//Movement overrides functions
	void BCIHandRightAndLeft(float value);
	void BCIHandForwardAndBackward(float value);
	void BCIHandUpAndDown(float value);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BCI")
		float mManualBCIHandMoveSpeedMultiplier = 2.f;
	//Offset the BCI hand 
	FVector mBCIHandOffset = FVector{ 0,0,0 };
	//Can input be taken
	
	bool bBCIInputEnabled = true;

	UFUNCTION(BlueprintCallable)
		EBCIMode GetBCIModeFromPlayer();

	UPROPERTY(EditAnywhere, Category = "BCI")
		TEnumAsByte<EBCIHandMovementMode> mBCIHandMovementMode = On_Input;

	//NETWORKING AND BCI INPUT
	//Parsing OSC messages
	UFUNCTION(BlueprintCallable)
		void RecieveOSCInputAddressAndFloat(FString address, float value);
	//Handle MentalCommandMovementInput
	void MentalCommandMovementInput(FVector direction, float strength);
	
	//Vector that moves the hand
	FVector mBCIHandMovementDirection = FVector{ 0, 0, 0 };
	//How much does mentalCommands affect movement
	float   mMentalCommandStrength = 0;
	//Decrease direction and strength over time ? 
	void HandleCachedBCIHandDirectionAndStrength();

#pragma endregion

#pragma region Grabbing Items
	//Grabbing stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mGrabRadius = 50.f;
	//Normal grabbing
	void Grab_Pressed();
	void Grab_Released();

	//VR grabbing
	void GripRightHand_Pressed();
	void GripRightHand_Released();
	void GripLeftHand_Pressed();
	void GripLeftHand_Released();

	class UGrabComponent* GetNearestGrabComponent(			FVector location);
	std::vector<UGrabComponent*> GetNearbyGrabComponents(	FVector location);
	
	std::unordered_map<UMotionControllerComponent*, UGrabComponent*> mGrabs;
	UGrabComponent* GetGrabbed(UMotionControllerComponent* mc);
	//Is the mc parameter currently holding an item
	bool GrabbingItem(UMotionControllerComponent* mc);

	UGrabComponent* mBCIGrabbed = nullptr;
#pragma endregion

#pragma region VR Teleportation
	//Teleporting
	void Teleport_Pressed();
	void Teleport_Released();

	void TeleportRightHand_Released();
	void TeleportRightHand_Pressed();
	void TeleportLeftHand_Pressed();
	void TeleportLeftHand_Released();

	bool TryTeleport();
	bool IsValidTeleportLocation(FHitResult hit);

	void TeleportVisuals();

	USceneComponent* mTeleportRayFrom;

	FHitResult GetTeleportAimHit();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* mTeleportGraphic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* mTeleportAimStart;
	class UStaticMeshComponent* mTeleportAimMesh;
	UPROPERTY(EditAnywhere)
		class UMaterial* mCanTeleportMat;
	UPROPERTY(EditAnywhere)
		class UMaterial* mCantTeleportMat;
#pragma endregion

#pragma region Interactions (VR and NORMAL)
	//For VR where both hand can be used for interaction
	void Right_Interact_Pressed();
	void Right_Interact_Released();
	void Left_Interact_Pressed();
	void Left_Interact_Released();
	void Interact_Pressed(UMotionControllerComponent* mc);
	void Interact_Released(UMotionControllerComponent* mc);

	//For Non-VR gameplay, these are called from the E button
	void Interact_Pressed();
	void Interact_Released();
	void InteractRaycastFromCamera();
	void InteractClosest();

	//BCI
	void Interact_BCIHand();
	class IInteractable* GetNearestInteractable(FVector location);
	std::vector<IInteractable*> GetNearbyInteractables(FVector location);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mInteractionRange = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bEnableInteractionHold = true;

#pragma endregion

private:
	bool bTeleporting = false;
	float mTeleportMaxRange = 1000.f;

	FVector mTeleportLocation;
};
