// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayer.h"
//General
#include "Peripheral.h"
#include "PeripheralGameInstance.h"
#include "MotionControllerComponent.h"

//Components
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GrabComponent.h"

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
	
	//Camera
	//mVROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VR_Origin_1"));
	//mCameraOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("Camera Origin"));
	//mVROrigin->SetupAttachment(GetCapsuleComponent());
	//mCameraOrigin->SetupAttachment(GetCapsuleComponent());
	////
	//mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//mCamera->SetupAttachment(mVROrigin);
	////	
	//// Create VR Controllers.
	//mRightMC = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right_MotionController"));
	//mRightMC->MotionSource = FXRMotionControllerBase::RightHandSourceId; //idk why this is done
	//mRightMC->SetupAttachment(mVROrigin);
	//
	//mLeftMC = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left_MotionController"));
	//mLeftMC->SetupAttachment(mVROrigin);

	//TODO : THIS MAY NOT BE USED; THIS IS TEMPORARY
	//mRightHandChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Right Hand Child Actor"));
	//mLeftHandChildActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Left Hand Child Actor"));
	////Set what class they are
	////TODO : If this is to be used, set this as the instantiation of the mRightHand and mLeftHand
	//mRightHandChildActor->SetChildActorClass(APeripheralHandActor::StaticClass());
	//mLeftHandChildActor->SetChildActorClass(APeripheralHandActor::StaticClass());

	////Spawn hands
	//mRightHand = CreateDefaultSubobject<APeripheralHandActor>(TEXT("Right Hand"));
	//mLeftHand  = CreateDefaultSubobject<APeripheralHandActor>(TEXT("Left Hand"));
	//
	//
	//
	//mRightHandChildActor->CreateChildActor();
	//
	//mRightHandChildActor->CreateChildActor();

	//mHandsMap[right] = mRightHand;
	//mHandsMap[left] = mLeftHand;

	//mHandsArray.Add(mRightHand);
	//mHandsArray.Add(mLeftHand);
	//
	//mHandsVector.push_back(mRightHand);
	//mHandsVector.push_back(mLeftHand);

	//Teleportation
	mTeleportGraphic = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportGraphic"));
	mTeleportAimStart = CreateDefaultSubobject<USceneComponent>(TEXT("TeleportAimStart"));

}

// Called when the game starts or when spawned
void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();
	//Find motion controllers if they are not spawned form here
	if (!mRightMC && !mLeftMC) {
		auto mcs = GetComponentsByClass(UMotionControllerComponent::StaticClass());
		for (auto mc : mcs) {
			if (mc->GetName() == "RightMC") {
				mRightMC = Cast<UMotionControllerComponent>(mc);
			}
			if (mc->GetName() == "LeftMC") {
				mLeftMC = Cast<UMotionControllerComponent>(mc);
			}

		}
	}
	if (!mCamera) {
		auto cams = GetComponentByClass(UCameraComponent::StaticClass());
		if (cams) {
			mCamera = Cast<UCameraComponent>(cams);
		}
	}
	

	mPeripheralGI = Cast<UPeripheralGameInstance>(GetGameInstance());
	if (!mPeripheralGI) {
		//Something is very wrong
	}
	//What mode are we in ? 
	auto mode = GetPeripheralMode();
	if (mode == NORMAL) {
		//We're using neither VR nor BCI, then we want to play as a regular fps game ? 
		bVR = false;
		mRightMC->SetHiddenInGame(true);
		mLeftMC->SetHiddenInGame(true);
	}
	if (mode == VR_BCI) {
		//Here we want normal VR with BCI hand override enabled

	}
	if (mode == VR) {
		//Normal VR without any overrides, 
		bVR = true;
	}
	if (mode == BCI) {
		//Here we want normal FPS controls, no VR, with a hand that can be overriden
		//mOverridenHand = mRightMC;
	}


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
	if (bVR) {
		//This should probably be changed to a better function
		//AlignHandAndMotionController(left);
		//AlignHandAndMotionController(right);
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

	InputComponent->BindAction("GrabRight", IE_Pressed, this, &AVRPlayer::GripRightHand_Pressed);
	InputComponent->BindAction("GrabRight", IE_Released, this, &AVRPlayer::GripRightHand_Released);

	InputComponent->BindAction("GrabLeft", IE_Pressed, this,   &AVRPlayer::GripLeftHand_Pressed);
	InputComponent->BindAction("GrabLeft", IE_Released, this,  &AVRPlayer::GripLeftHand_Released);

	InputComponent->BindAction("Teleport", IE_Pressed, this, &AVRPlayer::Teleport_Pressed);
	InputComponent->BindAction("Teleport", IE_Pressed, this, &AVRPlayer::Teleport_Released);


	InputComponent->BindAction("Right_Interact", IE_Pressed, this,  &AVRPlayer::Right_Interact_Pressed);
	InputComponent->BindAction("Right_Interact", IE_Released, this,  &AVRPlayer::Right_Interact_Pressed);

	InputComponent->BindAction("Left_Interact", IE_Pressed, this, &AVRPlayer::Left_Interact_Pressed);
	InputComponent->BindAction("Left_Interact", IE_Released, this, &AVRPlayer::Left_Interact_Pressed);

	InputComponent->BindAction("Interact", IE_Pressed, this, &AVRPlayer::Interact_Pressed);

	// Bind movement events
	InputComponent->BindAxis("MoveForward", this, &AVRPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AVRPlayer::MoveRight);

	InputComponent->BindAxis("Turn", this, &AVRPlayer::Turn);
	InputComponent->BindAxis("LookUp", this, &AVRPlayer::LookUp);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}
void AVRPlayer::SetCameraMode(EPeripheralMode mode)
{
	if (mode == VR || mode == VR_BCI) {
		mCamera->bLockToHmd = true;
		mCamera->bUsePawnControlRotation = false;
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Player : Set Camera To VR")));
	}
	else {
		mCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
		mCamera->bLockToHmd = false;

		mCamera->bUsePawnControlRotation = true;
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Player : Set Camera To NORMAL")));
	}
}
#pragma region FPS Controls
void AVRPlayer::MoveForward(float value)
{
	if (value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), value);
	}
}

void AVRPlayer::MoveRight(float value)
{
	if (value != 0.0f)
	{
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

#pragma region VR
void AVRPlayer::AlignHandAndMotionController(APeripheralHandActor* hand, UMotionControllerComponent* mc)
{
	if (!hand) {
		//Somethings very wrong
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("AlignHand no hand")));
		return;
	}
	if (!mc) {

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("AlignHand no mc")));
		return;
	}
	//TODO : THIS OFFSET SHOULD BE SET AS A PART OF THE "TRICK THE BRAIN" SEGMENT; USE GAME INSTANCE HERE MAYBE
	FVector offset(0, 0, 0);

	FVector loc = mc->GetComponentLocation();
	FRotator rot = mc->GetComponentRotation();

	loc += offset;

	hand->SetActorLocation(loc);
	hand->SetActorRotation(rot);
}
void AVRPlayer::AlignHandAndMotionController(EHandSide side)
{
	APeripheralHandActor* hand = nullptr;
	UMotionControllerComponent* mc = nullptr;
	if (side == right) {
		hand = mRightHand;
		mc = mRightMC;
	}
	if (side == right) {
		hand = mLeftHand;
		mc = mLeftMC;
	}
	if (!hand) {
		//Somethings very wrong
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("AlignHand no hand")));
		return;
	}
	if (!mc) {

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("AlignHand no mc")));
		return;
	}
	AlignHandAndMotionController(hand, mc);
}
bool AVRPlayer::StartVR()
{
	bVR = true;
	return true;
}

bool AVRPlayer::StopVR()
{
	bVR = false;
	return true;

}

#pragma endregion

#pragma region Grab
void AVRPlayer::GripRightHand_Pressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Right Hand Grip Pressed"));
	UE_LOG(LogTemp, Log, TEXT("Right Hand Grip Pressed"));
	auto grab = GetNearestGrabComponent(mRightMC);
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
	auto grab = GetNearestGrabComponent(mRightMC);
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
	auto grab = GetNearestGrabComponent(mLeftMC);
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

UGrabComponent* AVRPlayer::GetNearestGrabComponent(UMotionControllerComponent* mc)
{
	//Find all close by grabcomponents
	auto grabs = GetNearbyGrabComponents(mc);

	//Iterate trough distance
	UGrabComponent* nearest = nullptr;
	float distance = 1000.f;
	for (auto& grab : grabs) {
		//Get distance from
		float dist = FVector::Distance(grab->GetComponentLocation(), mc->GetComponentLocation());
		if (dist < distance) {
			nearest = grab;
			distance = dist;
		}
	}
	return nearest;
}

std::vector<UGrabComponent*> AVRPlayer::GetNearbyGrabComponents(UMotionControllerComponent* mc)
{
	//Object types
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));

	//What actors to ignore
	TArray<AActor*> ignored;
	ignored.Add(this);

	TArray<FHitResult> hits;
	bool bHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), mc->GetComponentLocation(), mc->GetComponentLocation(), 500.f,
		ObjectTypesArray, true, ignored, EDrawDebugTrace::ForDuration, hits, true);

	FCollisionShape col = FCollisionShape::MakeSphere(mGrabRadius);
	FVector loc = mc->GetComponentLocation();
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
	//Find interactaables around the mc
	auto inter = GetNearestInteractable(mc->GetComponentLocation());
	
	if (inter) {
		//We can interact with it
		inter->Interact(this);
	}
}

void AVRPlayer::Interact_Released(UMotionControllerComponent* mc)
{
}

void AVRPlayer::Interact_Pressed()
{
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

IInteractable* AVRPlayer::GetNearestInteractable(FVector location)
{
	//Find all close by grabcomponents
	auto interactables = GetNearbyInteractables(location);

	//Debug
	if (Debug()) {
		
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
