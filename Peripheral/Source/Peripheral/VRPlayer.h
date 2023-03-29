// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRPlayer.generated.h"

UENUM()
enum EHandSide{ right, left};

class UCameraComponent;
UCLASS()
class PERIPHERAL_API AVRPlayer : public ACharacter
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

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* mCamera;
	void SetCameraMode(enum EPeripheralMode mode);
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

	//Hands
	class APeripheralHandActor* mRightHand;
	APeripheralHandActor* mLeftHand;

	TMap<EHandSide, APeripheralHandActor*> mHandsMap;
	std::vector< APeripheralHandActor*> mHandsVector;
	TArray<APeripheralHandActor*> mHandsArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMotionControllerComponent* mRightMC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMotionControllerComponent* mLeftMC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UChildActorComponent* mRightHandChildActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UChildActorComponent* mLeftHandChildActor;
	void AlignHandAndMotionController(APeripheralHandActor* hand, UMotionControllerComponent* mc);
	void AlignHandAndMotionController(EHandSide side);
	//Simple VR mode mechanic
	bool IsVR() {
		return bVR;
	};
	bool StartVR();
	bool StopVR();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* mVROrigin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* mCameraOrigin;
	//Simple BCI mode mechanic
	bool UsingBCI() {
		return bUseBCI;
	}
	void SetUseBCI(bool bci) {
		bUseBCI = bci;
	}

	//Grabbing stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mGrabRadius = 200.f;
	void GripRightHand_Pressed();
	void GripRightHand_Released();
	void GripLeftHand_Pressed();
	void GripLeftHand_Released();

	class UGrabComponent* GetNearestGrabComponent(UMotionControllerComponent* mc);
	std::vector<UGrabComponent*> GetNearbyGrabComponents(UMotionControllerComponent* mc);
	
	std::unordered_map<UMotionControllerComponent*, UGrabComponent*> mGrabs;
	UGrabComponent* GetGrabbed(UMotionControllerComponent* mc);
	//Is the mc parameter currently holding an item
	bool GrabbingItem(UMotionControllerComponent* mc);

	//Teleporting
	void Teleport_Pressed();
	void Teleport_Released();
	bool TryTeleport();
	bool IsValidTeleportLocation(FHitResult hit);

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
	class IInteractable* GetNearestInteractable(FVector location);
	std::vector<IInteractable*> GetNearbyInteractables(FVector location);

	UPROPERTY(EditAnywhere)
		float mInteractionRange = 25.f;

private:
	bool bUseBCI = false;


	bool bVR = false;

	bool bTeleporting = false;
	float mTeleportMaxRange = 1000.f;


	FVector mTeleportLocation;
};
