// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayer.h"
//General
#include "PeripheralGameInstance.h"
#include "MotionControllerComponent.h"
#include "PeripheralHandActor.h"
//Components
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GrabComponent.h"
#include "BCIHandComponent.h"
//Misc
#include "Interactable.h"
#include "NavigationSystem.h"
#include "PeripheralHandActor.h"

//Libs
#include "Kismet/KismetSystemLibrary.h"

//VR
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "HeadMountedDisplayFunctionLibrary.h"

// Sets default values
AVRPlayer::AVRPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
	
	mRoot = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(mRoot);
	//Camera
	//mVROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VR_Origin_1"));
	//mCameraOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("Camera Origin"));
	//mVROrigin->SetupAttachment(GetCapsuleComponent());
	//mCameraOrigin->SetupAttachment(GetCapsuleComponent());
	////
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mCamera->SetupAttachment(RootComponent);
	////	
	//// Create VR Controllers.
	mRightMC = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right_MotionController"));
	mRightMC->MotionSource = "Right";
	//mRightMC->MotionSource = FXRMotionControllerBase::RightHandSourceId; //idk why this is done
	mRightMC->SetupAttachment(mVROrigin);
	
	mLeftMC = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left_MotionController"));
	mLeftMC->MotionSource = "Left";
	mLeftMC->SetupAttachment(mVROrigin);

	//Create BCI hand
	mBCIHand = CreateDefaultSubobject<UBCIHandComponent>(TEXT("BCI Hand"));
	mBCIHand->SetupAttachment(mRoot);

	//Teleportation
	mTeleportGraphic = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportGraphic"));
	mTeleportAimStart = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportAimStart"));
}

// Called when the game starts or when spawned
void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();

	mPeripheralGI = Cast<UPeripheralGameInstance>(GetGameInstance());
	if (!mPeripheralGI) {
		//Something is very wrong
	}
	//What mode are we in ? 
	auto mode = GetPeripheralMode();
	if (mode == NORMAL) {
		//We're using neither VR nor BCI, then we want to play as a regular fps game ? 
	
		mRightMC->SetHiddenInGame(true);
		mLeftMC->SetHiddenInGame(true);
		bUseControllerRotationYaw = true;
	}
	
	if (mode == VR) {
		//Normal VR without any overrides, 
		
	}
	auto bciMode = GetBCIMode();
	SetBCIMode(bciMode);
	SetCameraMode(mode);

	//What do we want to do different here
	mTeleportGraphic->SetHiddenInGame(true);

	auto comps = GetComponents();

	TArray<USceneComponent*> sceneComps;
	mTeleportGraphic->GetChildrenComponents(false, sceneComps);

	for (auto comp : sceneComps) {
		auto mesh = Cast<UStaticMeshComponent>(comp);
		if (mesh) {
			mTeleportAimMesh = mesh;
		}
	}
}

// Called every frame
void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//If we're using VR, we want our hands to be aligned with the motion controllers

	if (bDebug) {
		Debug();
	}
	switch (mBCIHandMovementMode) {
	case On_Input:

		break;
	case On_Update:
		//Moves hand with cached direction and strength
		MoveBCIHand(mBCIHandMovementDirection, mMentalCommandStrength);
		//
		HandleCachedBCIHandDirectionAndStrength();
		break;
	}

	if (bTeleporting) {
		auto hit = GetTeleportAimHit();
		if (hit.GetActor()) {
			mTeleportLocation = GetTeleportAimHit().Location;
		}
		mTeleportGraphic->SetWorldLocation(hit.Location);

		//Change colors baed on if its legal or not
		bool legal = IsValidTeleportLocation(hit);
		if (legal) {
			GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, FString::Printf(TEXT("Teleport")));
			mTeleportAimMesh->SetMaterial(0, mCanTeleportMat);
		}
		else {
			mTeleportAimMesh->SetMaterial(0, mCantTeleportMat);
			GEngine->AddOnScreenDebugMessage(10, 1, FColor::Red, FString::Printf(TEXT("Teleport")));
		}
	}
}

// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind movement events
	InputComponent->BindAxis("MoveForward", this, &AVRPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AVRPlayer::MoveRight);

	InputComponent->BindAxis("Turn", this, &AVRPlayer::Turn);
	InputComponent->BindAxis("LookUp", this, &AVRPlayer::LookUp);

	//Teleportation for VR Controlelrs
	InputComponent->BindAction("Teleport", IE_Pressed, this, &AVRPlayer::Teleport_Pressed);
	InputComponent->BindAction("Teleport", IE_Pressed, this, &AVRPlayer::Teleport_Released);

	//Grabbing for VR Controllers
	InputComponent->BindAction("GrabRight", IE_Pressed, this, &AVRPlayer::GripRightHand_Pressed);
	InputComponent->BindAction("GrabRight", IE_Released, this, &AVRPlayer::GripRightHand_Released);

	InputComponent->BindAction("GrabLeft", IE_Pressed, this,   &AVRPlayer::GripLeftHand_Pressed);
	InputComponent->BindAction("GrabLeft", IE_Released, this,  &AVRPlayer::GripLeftHand_Released);

	//Interaction for VR Controllers
	InputComponent->BindAction("Right_Interact", IE_Pressed, this,  &AVRPlayer::Right_Interact_Pressed);
	InputComponent->BindAction("Right_Interact", IE_Released, this,  &AVRPlayer::Right_Interact_Pressed);

	InputComponent->BindAction("Left_Interact", IE_Pressed, this, &AVRPlayer::Left_Interact_Pressed);
	InputComponent->BindAction("Left_Interact", IE_Released, this, &AVRPlayer::Left_Interact_Pressed);

	//Interaciton for Normal & BCI mode
	InputComponent->BindAction("Interact", IE_Pressed, this, &AVRPlayer::Interact_Pressed);
	InputComponent->BindAction("Interact", IE_Released, this, &AVRPlayer::Interact_Released);

	//BCI modes
	InputComponent->BindAction("SwapBCIMode", IE_Pressed, this, &AVRPlayer::SwapGlobalBCIMode);
	InputComponent->BindAction("SwapBCIInputEnabled", IE_Pressed, this, &AVRPlayer::SwapBCIInputEnabled);
	//Overriden movement for BCI
	InputComponent->BindAxis("RightLeft", this, &AVRPlayer::BCIHandRightAndLeft);
	InputComponent->BindAxis("ForwardBackward", this, &AVRPlayer::BCIHandForwardAndBackward);
	InputComponent->BindAxis("UpDown", this, &AVRPlayer::BCIHandUpAndDown);

	//Manual grabbing buttons
	InputComponent->BindAction("Grab", IE_Pressed, this, &AVRPlayer::Grab_Pressed);
	InputComponent->BindAction("Grab", IE_Released, this, &AVRPlayer::Grab_Released);

}

void AVRPlayer::Debug() {



}

void AVRPlayer::SetCameraMode(EPeripheralMode mode)
{
	switch (mode) {
	case NORMAL:
		//Camera should not be locked to the HeadMountedDispaly
		mCamera->bLockToHmd = false;
		//The camera should use pawn rotation
		mCamera->bUsePawnControlRotation = true;
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Player : Set Camera To NORMAL")));
		break;
	case VR:
		//Lock camera to HMD
		mCamera->bLockToHmd = true;
		//Dont rotate pawn based on camera / vice versa
		mCamera->bUsePawnControlRotation = false;
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Player : Set Camera To VR")));
		break;
	}
}

#pragma region FPS Controls
void AVRPlayer::MoveForward(float value)
{
	if (value != 0.0f)
	{
		FVector dir = bFlyingCamera ? mCamera->GetForwardVector() : GetActorForwardVector();
		// add movement in that direction
		AddMovementInput(dir, value);
	}
}

void AVRPlayer::MoveRight(float value)
{
	if (value != 0.0f)
	{
		FVector dir = bFlyingCamera ? mCamera->GetRightVector() : GetActorRightVector();
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), value);
	}
}
void AVRPlayer::Turn(float value)
{
	AddControllerYawInput(value * mTurnRate * GetWorld()->GetDeltaSeconds());
}
void AVRPlayer::LookUp(float value)
{
	AddControllerPitchInput(value * mLookUpRate * GetWorld()->GetDeltaSeconds());
}
#pragma endregion

#pragma region BCI
void AVRPlayer::ConfigureBCIMode(EBCIMode mode)
{
	switch (mode) {
	case NO_BCI:
		//What should happend here ?
		//Disable BCI hand
		mBCIHand->Configure(false);
		break;
	case BCI:
		//What should happend here ?
		//Enable BCI hand
		mBCIHand->Configure(true);
		//shoudl the BCI hand be parented to the camera ? 
		bool parentCamera = GetPeripheralMode() == NORMAL;//if mode equals normal, the parent it to camera, in vr it should be parented to body
		if (parentCamera) {
			mBCIHand->AttachToComponent(mCamera, FAttachmentTransformRules::KeepWorldTransform);
			mBCIHand->SetRelativeLocation(FVector(100, 0, 0));
		}
		else {
			mBCIHand->SetupAttachment(mRoot);

		}
		break;
	}
}
//Swaps between non-bci and bci mode
void AVRPlayer::SwapGlobalBCIMode() {
	auto mode = GetBCIMode();
	if (mode == NO_BCI) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("BCI-mode set to BCI")));
		SetBCIMode(BCI);
	}
	if (mode == BCI) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("BCI-mode set to NO_BCI")));
		SetBCIMode(NO_BCI);
	}
	ConfigureBCIMode(GetBCIMode());
}
//Enables or disables Input to BCI HAnd
void AVRPlayer::SwapBCIInputEnabled()
{
	bBCIInputEnabled = !bBCIInputEnabled;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("BCI Input set to %d"), bBCIInputEnabled));
}

void AVRPlayer::MoveBCIHand(FVector dir, float strength)
{
	float deltaSeconds = GetWorld()->GetDeltaSeconds();

	auto forward = mBCIHand->GetForwardVector();
	auto right = mBCIHand->GetRightVector();
	auto pos = mBCIHand->GetComponentLocation();

	auto speed = mBCIHandMoveSpeed * deltaSeconds * (bUseInputStrength ? strength : 1);
	auto delta = dir * speed;

	mBCIHand->AddRelativeLocation(delta);
}

void AVRPlayer::BCIHandRightAndLeft(float value)
{
	FVector dir = FVector{ 0, value * mManualBCIHandMoveSpeedMultiplier, 0 };
	MoveBCIHand(dir, 1);
}

void AVRPlayer::BCIHandForwardAndBackward(float value)
{
	FVector dir = FVector{ value * mManualBCIHandMoveSpeedMultiplier, 0, 0 };
	MoveBCIHand(dir, 1);
}

void AVRPlayer::BCIHandUpAndDown(float value)
{
	FVector dir = FVector{ 0, 0, value * mManualBCIHandMoveSpeedMultiplier };
	MoveBCIHand(dir, 1);
}

void AVRPlayer::RecieveOSCInputAddressAndFloat(FString address, float value) {
	//direction for commands
	TMap<FString, FVector> directions = {
	{"right"	, FVector( 0, 1, 0)},
	{"left"		, FVector( 0,-1, 0)},
	{"forward"	, FVector( 1, 0, 0)},
	{"backward"	, FVector(-1, 0, 0)},
	{"up"		, FVector( 0, 0, 1)},
	{"down"		, FVector( 0, 0,-1)},
	{"neutral"	, FVector( 0, 0, 0)},
	};
	
	//Parse string
	//The address is formated as /info/command
	std::string string = std::string(TCHAR_TO_UTF8(*address));
	std::vector<int> seperators;
	for (int i = 0; i < string.size(); i++) {
		if (string[i] == '/') {
			seperators.push_back(i);
		}
	}
	if (seperators.size() != 2) {
		//Something is worng 
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Player : Parsing OSC String Did not Find 2 Seperators")));
		return;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Player : Seperators %d, %d"), seperators[0], seperators[1]));
	//Type is defined between the seperators
	FString type = address.RightChop(seperators[0]+1);
	type = type.LeftChop(type.Len() - seperators[1] +1);
	auto allowedTypes = std::vector<FString>{ "neuropype", "com" };//nueropype prediction, and mental command from emotiv
	for (int i = 0; i < allowedTypes.size(); i++) {
		if (type == allowedTypes[i]) {// Is
			GEngine->AddOnScreenDebugMessage(800, 5, FColor::Green, FString::Printf(TEXT("Player : Correct address found : %s"), *type.ToUpper()));
			break; //Exit loop, found correct one
		}
		//Have gone trough all allowed types but none were correct, then something is wrong
		if (i == allowedTypes.size() - 1) {
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Found no correct OSC address : %s"), *type));
			return;
		}
	}

	//Command
	FString command  = address.RightChop(seperators[1]+1);
	if (!directions.Find(command)) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Player : %s is not recognized"), *command.ToUpper()));
		return;
	}
	FVector dir = directions[command];
	GEngine->AddOnScreenDebugMessage(350, 5, FColor::Green, FString::Printf(TEXT("Player : Command : %s with value %f: "), *command.ToUpper(), value));

	//GEngine->AddOnScreenDebugMessage(350, 5, FColor::Green, FString::Printf(TEXT("Player : Command : %s with (%d, %d, %d)"), *command.ToUpper(), dir.X, dir.Y, dir.Z));
	MentalCommandMovementInput(dir, value);	
}
void AVRPlayer::MentalCommandMovementInput(FVector direction, float strength)
{
	//Dont apply if we're not in BCI mode
	auto mode = GetBCIMode();
	if (mode != BCI) {
		return;
	}
	//Check for disabled input
	if (!bBCIInputEnabled) {
		return;
	}
	switch (mBCIHandMovementMode) {
	case On_Input:
		MoveBCIHand(direction, strength);
		break;
	case On_Update:
		mBCIHandMovementDirection = direction;
		mMentalCommandStrength = strength;
		break;
	}
	
}
void AVRPlayer::HandleCachedBCIHandDirectionAndStrength()
{
	float deltaSeconds = GetWorld()->GetDeltaSeconds();
	//How much to decrease / increase per second
	float decreaseRate = 0.05;

	//Decrease strength each seconds, so if the input stops or is neutral it decreases movement
	mMentalCommandStrength -= decreaseRate * deltaSeconds;
	//Dont go below zero
	if (mMentalCommandStrength < 0) {
		mMentalCommandStrength = 0;
	}
}
#pragma endregion


#pragma region VR
#pragma endregion

#pragma region Grab
void AVRPlayer::GripRightHand_Pressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Right Hand Grip Pressed"));
	UE_LOG(LogTemp, Log, TEXT("Right Hand Grip Pressed"));
	auto grab = GetNearestGrabComponent(mRightMC->GetComponentLocation());
	if (!grab) {
		return;
	}

	if (mGrabs[mLeftMC] == grab) {
		if (!mGrabs[mLeftMC]->TryRelease()) {
			//Some error 
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Right hand has grabbed an item which wont release from the left %f"), 5));
			return;
		}
	}

	bool grabbed = grab->TryGrab(mRightMC);
	if (!grabbed) {
		return;
	}
	mGrabs[mRightMC] = grab;

}
void AVRPlayer::GripRightHand_Released()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Right Hand Grip Released"));
	UE_LOG(LogTemp, Log, TEXT("Rigth Hand Grip Released"));
	auto grab = GetNearestGrabComponent(mRightMC->GetComponentLocation());
	if (!grab) {
		return;
	}
	bool released = grab->TryRelease();
	if (!released) {
		return;
	}
	mGrabs[mRightMC] = nullptr;
}
void AVRPlayer::GripLeftHand_Pressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Left Hand Grip Pressed"));
	UE_LOG(LogTemp, Log, TEXT("Left Hand Grip Pressed"));
	auto grab = GetNearestGrabComponent(mLeftMC->GetComponentLocation());
	if (!grab) {
		return;
	}

	if (mGrabs[mRightMC] == grab) {
		if (!mGrabs[mRightMC]->TryRelease()) {
			//Some error 
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Right hand wont release the component %f"), 5));
			return;
		}
	}
	bool grabbed = grab->TryGrab(mLeftMC);
	if (!grabbed) {
		return;
	}
	//Grabbed it
	mGrabs[mLeftMC] = grab;

}
void AVRPlayer::GripLeftHand_Released()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Left Hand Grip Released"));
	UE_LOG(LogTemp, Log, TEXT("Left Hand Grip Released"));
	if (!mGrabs[mLeftMC]) {
		return;
	}

	bool released = mGrabs[mLeftMC]->TryRelease();
	if (!released) {
		return;
	}
	mGrabs[mLeftMC] = nullptr;
}

void AVRPlayer::Grab_Pressed()
{
	auto mode = GetBCIMode();
	if (mode != BCI) {
		//Do something else
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Player : Not grabbing beacuse not in BCI mode")));
		return;
	}
	//We know for sure we're in BCI mode
	//Get closest grab component to BCI hand
	auto grab = GetNearestGrabComponent(mBCIHand->GetComponentLocation());
	if (!grab) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Player : No grabables near BCI Hand")));
		return;
	}
	auto grabbed = grab->ForceGrab(mBCIHand, true);
	if (!grabbed) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Player : Grab did not accept!")));
		return;
	}
	mBCIGrabbed = grab;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Player : BCI Hand Grabbed %s"), *grab->GetOwner()->GetName()));

}

void AVRPlayer::Grab_Released()
{
	if (!mBCIGrabbed) {
		return;
	}
	mBCIGrabbed->ForceReleased(mBCIHand, true);

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Player : BCI Hand released %s"), *mBCIGrabbed->GetOwner()->GetName()));
	mBCIGrabbed = nullptr;

}

UGrabComponent* AVRPlayer::GetNearestGrabComponent(FVector location)
{
	//Find all close by grabcomponents
	auto grabs = GetNearbyGrabComponents(location);

	//Iterate trough distance
	UGrabComponent* nearest = nullptr;
	float distance = 1000.f;
	for (auto& grab : grabs) {
		//Get distance from
		float dist = FVector::Distance(location, grab->GetComponentLocation());
		if (dist < distance) {
			nearest = grab;
			distance = dist;
		}
	}
	return nearest;
}

std::vector<UGrabComponent*> AVRPlayer::GetNearbyGrabComponents(FVector location)
{
	//Object types
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));

	//What actors to ignore
	TArray<AActor*> ignored;
	ignored.Add(this);

	TArray<FHitResult> hits;
	bool bHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), location, location, 500.f,
		ObjectTypesArray, true, ignored, EDrawDebugTrace::ForDuration, hits, true);

	FCollisionShape col = FCollisionShape::MakeSphere(mGrabRadius);
	FVector loc = location;
	bool isHit = GetWorld()->SweepMultiByChannel(hits, loc, loc, FQuat::Identity, ECC_PhysicsBody, col);
	std::vector<UGrabComponent*> grabs;
	for (auto& hit : hits) {
		auto actor = hit.GetActor();

		//Check if its has a grab component
		auto comps = actor->GetComponentsByClass(UGrabComponent::StaticClass());
		for (auto& comp : comps) {
			UGrabComponent* grab = Cast<UGrabComponent>(comp);
			if (grab) {
				grabs.push_back(grab);
			}
		}
	}
	
	return grabs;
}

UGrabComponent* AVRPlayer::GetGrabbed(UMotionControllerComponent* mc)
{
	return mGrabs[mc];
}

bool AVRPlayer::GrabbingItem(UMotionControllerComponent* mc)
{
	if (GetGrabbed(mc)) {
		return true;
	}
	else {
		return false;
	}
}
#pragma endregion

#pragma region Teleportation
void AVRPlayer::Teleport_Pressed()
{
	bTeleporting = true;

	mTeleportGraphic->SetHiddenInGame(false);
}

void AVRPlayer::Teleport_Released()
{
	bTeleporting = false;

	bool canTeleport = TryTeleport();
	if (!canTeleport) {
		return;
	}

	SetActorLocation(mTeleportLocation);


	mTeleportGraphic->SetHiddenInGame(true);
}

bool AVRPlayer::TryTeleport()
{
	//Find where we are aiming from

	auto Hit = GetTeleportAimHit();
	if (!Hit.GetActor()) {
		return false;
	}


	if (IsValidTeleportLocation(Hit)) {
		mTeleportLocation = Hit.Location;
		return true;
	}

	return false;
}

bool AVRPlayer::IsValidTeleportLocation(FHitResult hit)
{
	FNavLocation OutLocation;
	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	auto bHitNav = NavSystem->ProjectPointToNavigation(hit.Location,OutLocation);
	return true;
}

FHitResult AVRPlayer::GetTeleportAimHit()
{
	//Cast ray from aim start
	auto rayStart = mTeleportAimStart->GetComponentLocation();
	auto forw = mTeleportAimStart->GetForwardVector();
	FVector rayEnd = rayStart + forw * mTeleportMaxRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit, rayStart, rayEnd, ECC_WorldDynamic, QueryParams); // simple trace function
	return Hit;
}


#pragma endregion

#pragma region Interaction

#pragma region Right n Left 
void AVRPlayer::Right_Interact_Pressed()
{
	Interact_Pressed(mRightMC);
}

void AVRPlayer::Right_Interact_Released()
{
	Interact_Released(mRightMC);
}

void AVRPlayer::Left_Interact_Pressed()
{
	Interact_Pressed(mLeftMC);
}

void AVRPlayer::Left_Interact_Released()
{
	Interact_Released(mLeftMC);
}
#pragma endregion

void AVRPlayer::Interact_Pressed(UMotionControllerComponent* mc)
{
	if (!mc) {
		return;
	}
	//Find interactaables around the mc
	auto inter = GetNearestInteractable(mc->GetComponentLocation());
	if (!inter) {
		return;
	}
	//We can interact with it
	inter->Interact(this);

}

void AVRPlayer::Interact_Released(UMotionControllerComponent* mc)
{
}

void AVRPlayer::Interact_Pressed()
{
	auto mode = GetPeripheralMode();
	auto bciMode = GetBCIMode();
	if (bciMode == BCI) {
		//Find interactaables around the mc
		auto inter = GetNearestInteractable(mBCIHand->GetComponentLocation());
		if (!inter) {
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Player : No interactables close to hand!")));
			return;
		}
		//We can interact with it
		inter->Interact(this);
		return;
	}
	switch (mode) {
	case NORMAL:
		//In normal mode we want to 

		break;
	case VR:
		
		break;
	}
	auto start = mCamera->GetComponentLocation();
	auto end = mCamera->GetComponentLocation() + (mCamera->GetForwardVector() * mInteractionRange);

	//Normal interact has been pressed, what do we do ? 
	TArray<FHitResult> hits;
	FHitResult hit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(hit,  start, end, ECC_Visibility, QueryParams);

	//Vector to store itneractables found
	std::vector<IInteractable*> interactables;

	if (IsValid(hit.GetActor())) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Player : Hit %s"), *hit.GetActor()->GetName()));

		//check if we get a interactable component from here
		auto inter = Cast<IInteractable>(hit.GetActor());
		if (inter) {
			inter->Interact(this);
			return;

			interactables.push_back(inter);
		}

		auto comps = hit.GetActor()->GetComponentsByInterface(UInteractable::StaticClass());
		for (auto comp : comps) {
			auto interactableComp = Cast<IInteractable>(comp);
			if (interactableComp) {
				interactables.push_back(interactableComp);
			}
			
		}
	}
	float amount = interactables.size();
	//Debug amount of itneratables
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Player : Hit %f interactables"), amount));

	//there are no 
	if (interactables.size() == 0) {
		return;
	}
	IInteractable* interactable = interactables[0];

	float maxDistance = FVector::Distance(hit.Location, interactable->GetInteractableLocation());;
	//Take the closest to where the hit landed
	for (auto inter : interactables) {
		auto distance = FVector::Distance(hit.Location, inter->GetInteractableLocation());
		if (distance < maxDistance) {
			maxDistance = distance;
			interactable = inter;
		}
	}

	//Intertact with it
	interactable->Interact(this);
	
}

void AVRPlayer::Interact_Released()
{

}

void AVRPlayer::Interact_BCIHand()
{
}

IInteractable* AVRPlayer::GetNearestInteractable(FVector location)
{
	//Find all close by grabcomponents
	auto interactables = GetNearbyInteractables(location);

	//Debug
	if (bDebug) {
		
	}
	//Iterate trough distance
	IInteractable* nearest = nullptr;
	float distance = 1000.f;
	for (auto& interact : interactables) {
		//Where is the interactable
		FVector interactLocation = interact->GetInteractableLocation();
		
		//Get distance from
		float dist = FVector::Distance(interactLocation, location);
		if (dist < distance) {
			nearest = interact;
			distance = dist;
		}
	}
	return nearest;
}

std::vector<IInteractable*> AVRPlayer::GetNearbyInteractables(FVector location)
{
	//Object types
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));

	//What actors to ignore
	TArray<AActor*> ignored;
	ignored.Add(this);

	TArray<FHitResult> hits;
	bool bHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), location, location, 500.f,
		ObjectTypesArray, true, ignored, EDrawDebugTrace::ForDuration, hits, true);

	FCollisionShape col = FCollisionShape::MakeSphere(mInteractionRange);
	FVector loc = location;
	bool isHit = GetWorld()->SweepMultiByChannel(hits, loc, loc, FQuat::Identity, ECC_PhysicsBody, col);
	std::vector<IInteractable*> inters;
	for (auto& hit : hits) {
		auto actor = hit.GetActor();
		//Is the actor itself an interactable ? 
		auto inter = Cast<IInteractable>(actor);
		if (inter) {
			inters.push_back(inter);
		}
		//Check its component for interactales
		auto comps = actor->GetComponents();
		for (auto& comp : comps) {
			auto interactable = Cast<IInteractable>(comp);
			if (interactable) {
				inters.push_back(interactable);
			}
		}
	}

	return inters;
}


#pragma endregion
