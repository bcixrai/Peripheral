// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "AnimalInteractionComponent.h"
#include "VRPlayer.h"
#include "Ball.h"
// Sets default values
AAnimal::AAnimal()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mBallHolder = CreateDefaultSubobject<USceneComponent>(TEXT("Ball Holder"));
	mBallHolder->SetupAttachment(RootComponent);
	mBallMesh = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));
	mBallMesh->SetupAttachment(mBallHolder);
}

// Called when the game starts or when spawned
void AAnimal::BeginPlay()
{
	Super::BeginPlay();
	
	auto mesh = GetMesh();
	if (mesh) {
		if (!mAnimInstanceBP) {
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Animal : No Anim Instance BP"));
		}
		else {
			mesh->AnimClass = mAnimInstanceBP;
		}
	}
	mBallMesh->SetHiddenInGame(true);
	mMode = WANDER;
	NewWanderPoint();
}

// Called every frame
void AAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (mMode) {
	case WANDER:

		Wander(DeltaTime);
		break;
	case PLAYER:
		//What should the we do when in player mode ? 
		FollowPlayer(DeltaTime);
		break;
	case IDLE:
		Idle(DeltaTime);
		break;
	case BALL:
		Ball(DeltaTime);
		break;
	case TAUNT:
		Taunt(DeltaTime);
		break;
	}


	//IF we have the ball, the whole catch and deliver sequence is complete, and the animal will act normal again, and drop the ball after a time
	if (mBallMode == HASBALL) {
		mCurrentBallTime += DeltaTime;
		if (mCurrentBallTime > mDropBallAfterTime) {
			//Drop Ball
			DropBall();
		}
	}
}

// Called to bind functionality to input
void AAnimal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAnimal::Idle(float DeltaTime)
{
	auto dist = DistanceFromPlayer();
	if (dist > mPlayerIdleDistance) {
		mMode = PLAYER;
	}

	//TODO : Set idle animation
}

void AAnimal::InteractWithPlayer(UAnimalInteractionComponent* comp, EAnimalInteractionType reaction)
{
	if (mMode == TAUNT) {
		return;
	}
	//When can we begin to taunt
	if (mMode == PLAYER || mMode == IDLE || (mMode == BALL && mBallMode != CATCH && mBallMode != DELIVER)) {
		mMode = TAUNT;

		mCurrentTauntTime = 0;

		mTauntType = reaction;
	}
}

void AAnimal::Taunt(float DeltaTime)
{
	mCurrentTauntTime += DeltaTime;

	GEngine->AddOnScreenDebugMessage(10, 10, FColor::Green, FString::Printf(TEXT("Animal : TauntTime : %f"), mCurrentTauntTime));
	if (mCurrentTauntTime > mTauntTime) {
		//Reset timer
		mCurrentTauntTime = 0;
		//Return to idle and return to normal behavoir from there
		mMode = IDLE;

		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Animal : Stopped Taunt")));
		return;
	}

	//Make sure to set animation
}

void AAnimal::FollowPlayer(float DeltaTime)
{
	//Are we close enough to idle by the player?
	auto dist = DistanceFromPlayer();
	if (dist < mPlayerIdleDistance) {
		mMode = IDLE;
		return;
	}
	if (dist > mPlayerNoticeDistance) {
		mMode = WANDER;
		NewWanderPoint();
		return;
	}

	//Where do we want to be ? 
	//Do we want to go infornt of the player all the time or just go to them

	//if infront of the player is on the nav mesh, choose that

	//If its not on the navmesh, just go to the closest available
	//Find the direction from the player towards the animal
	auto direction = GetActorLocation() - mPlayer->GetActorLocation();
	direction.Normalize();
	//We want a point close to the player from that direction
	auto pos = mPlayer->GetActorLocation() + (direction * (mPlayerIdleDistance * 0.8f));

	//We know need a new direction from the pos
	auto newDir = pos - GetActorLocation();
	newDir.Normalize();
	//Apply new dir
	AddActorWorldOffset(newDir * DeltaTime * mFollowPlayerSpeed);

	//Look at the player, this will do wired stuff, but x rotation can be locked from blueprint, remeber this
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mPlayer->GetActorLocation()));
}

float AAnimal::DistanceFromPlayer()
{
	if (!mPlayer) {
		return 0;
	}
	return FVector::Distance(GetActorLocation(), mPlayer->GetActorLocation());
}

void AAnimal::Wander(float DeltaTime)
{
	auto playerDist = DistanceFromPlayer();
	if (playerDist < mPlayerNoticeDistance) {
		mMode = PLAYER;
		return;
	}

	float dist = DistanceFromRoamTarget();
	GEngine->AddOnScreenDebugMessage(1, 5, FColor::Cyan, FString::Printf(TEXT("Animal : Wander Dist %f"), dist));
	if (dist < mWanderTolerance || mWanderTarget == FVector{0,0,0}) {

		NewWanderPoint();
		return;
	}
	//Move towards wander point
	AddActorWorldOffset(mWanderDirection * mWanderSpeed * DeltaTime);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mWanderTarget));
	
}

float AAnimal::DistanceFromRoamTarget()
{
	return FVector::Distance(GetActorLocation(), mWanderTarget);
}

void AAnimal::NewWanderPoint()
{
	//new wander point
	auto loc = GetActorLocation();
	auto circleLocaction = loc + (GetActorForwardVector() * mWanderTargetDistance);

	circleLocaction.X += mWanderRadius * cos(FMath::DegreesToRadians(FMath::RandRange(-90, 90)));
	circleLocaction.Y += mWanderRadius * sin(FMath::DegreesToRadians(FMath::RandRange(-90, 90)));

	mWanderTarget =
		mWanderTarget = UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), GetActorLocation(), mWanderRadius);;

	mWanderDirection = mWanderTarget - GetActorLocation();
	mWanderDirection.Normalize();

}

void AAnimal::Ball(float DeltaTime)
{
	switch (mBallMode) {
	case CATCH:
		Catch(DeltaTime);
		break;
	case DELIVER:
		Deliver(DeltaTime);
		break;
	}
}

void AAnimal::Catch(float DeltaTime)
{

	if (!mBall) {
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Animal : No Ball")));
		return;
	}

	auto dir = mBall->GetActorLocation() - GetActorLocation();
	dir.Normalize();

	//Move toward ball
	AddActorWorldOffset(dir * DeltaTime * mCatchBallSpeed);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mBall->GetActorLocation()));

	//Can we pick up the ball ? 
	auto dist = FVector::Distance(GetActorLocation(), mBall->GetActorLocation());
	if (dist < mCatchBallTolerance) {
		PickupBall();
	}
}

void AAnimal::Deliver(float DeltaTime)
{
	//If its not on the navmesh, just go to the closest available
	//Find the direction from the player towards the animal
	auto direction = GetActorLocation() - mPlayer->GetActorLocation();
	direction.Normalize();
	//We want a point close to the player from that direction
	auto pos = mPlayer->GetActorLocation() + (direction * (mPlayerIdleDistance * 0.8f));

	//We know need a new direction from the pos
	auto newDir = pos - GetActorLocation();
	newDir.Normalize();
	//Apply new dir
	AddActorWorldOffset(newDir * DeltaTime * mDeliverBallSpeed);

	//Look at the player, this will do wired stuff, but x rotation can be locked from blueprint, remeber this
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mPlayer->GetActorLocation()));

	auto dist = DistanceFromPlayer();
	if (dist < mPlayerIdleDistance) {
		//Re enter idle mode
		mMode = IDLE;
		//The ball has now been delivered as far as the animal is conecered, it will hold on to it for a certain amount of time, then drop it, no matter hwat happens
		mBallMode = HASBALL;

		DropBall();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Animal : Finished Delivery"));
	}
}


bool AAnimal::NoticeBall(AActor* ball)
{
	mMode = BALL;
	mBallMode = CATCH;

	mBall = ball;
	return true;
}

void AAnimal::PickupBall()
{
	//The ball has been picked up, no we deliver it
	mBallMode = DELIVER;
	
	if (!mBall) {
		return;
	}
	//KIll the picked up ball
	mBall->Destroy();
	mBall = nullptr;

	//Enable ball mesh
	mBallMesh->SetHiddenInGame(false);
}

void AAnimal::DropBall()
{
	//Turn off ball mesh
	mBallMesh->SetHiddenInGame(true);

	//Spawn ball at holding position
	FActorSpawnParameters SpawnInfo;
	auto ball = Cast<ABall>(GetWorld()->SpawnActor(mBallBP));
	if (ball) {
		ball->SetActorLocation(mBallHolder->GetComponentLocation() + mBallSpawnOffset);
	}

	mBallMode = NOBALL;

	mBall = nullptr;
}

float AAnimal::GetCurrentSpeed()
{
	if (mMode == IDLE) {
		return 0.f;
	}

	if (mMode == PLAYER) {
		return mFollowPlayerSpeed;
	}
	if (mMode == BALL) {
		if (mBallMode == CATCH) {
			return mCatchBallSpeed;
		}
		if (mBallMode == DELIVER) {
			return mDeliverBallSpeed;
		}
	}

	if (mMode == WANDER) {
		return mWanderSpeed;
	}
	return 0.0f;
}

void AAnimal::Test()
{
	InteractWithPlayer(nullptr, HAPPY);
}
