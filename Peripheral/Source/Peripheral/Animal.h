// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Peripheral.h"
#include "Animal.generated.h"


UCLASS()
class PERIPHERAL_API AAnimal : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAnimal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf< UAnimInstance> mAnimInstanceBP;

	UFUNCTION(BlueprintCallable)
		void SetPlayer(AActor* player) {
		mPlayer = player;
	}
	UFUNCTION(BlueprintCallable)
		AActor* GetPlayer() {
		return mPlayer;
	}
	//Idiling
	void Idle(float DeltaTime);

	//Taunting
	UFUNCTION(BlueprintCallable)
	void InteractWithPlayer(class UAnimalInteractionComponent* comp, EAnimalInteractionType reaction);
	
	void Taunt(float DeltaTime);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mTauntTime = 20.f;
	float mCurrentTauntTime = 0.f;
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		float mTauntCooldown = 5.f;
	float mCurrentTauntCooldown = 0;
	EAnimalInteractionType mTauntType = HAPPY;
	EAnimalInteractionType GetCurrentTauntType() {
		return mTauntType;
	};
	//Player
	void FollowPlayer(float DeltaTime);
	float DistanceFromPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mPlayerNoticeDistance = 600.f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mPlayerIdleDistance = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mFollowPlayerSpeed = 100.f;

	//Wandering
	void Wander(float DeltaTime);
	float DistanceFromRoamTarget();
	void NewWanderPoint();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mWanderTargetDistance = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mWanderRadius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mWanderTolerance = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mWanderSpeed = 100.f;

	//Ball
	void Ball(float DeltaTime);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> mBallBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* mBallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector mBallSpawnOffset;
	AActor* mBall;
	UFUNCTION(BlueprintCallable)
	bool NoticeBall(AActor* ball);
	void PickupBall();
	void DropBall();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mDropBallAfterTime = 5.f;
	float mCurrentBallTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* mBallHolder;
	//Catch
	void Catch(float DeltaTime);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mCatchBallTolerance = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mCatchBallSpeed = 200.f;

	//Deliver
	void Deliver(float DeltaTime);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float mDeliverBallSpeed = 200.f;

	UFUNCTION(BlueprintCallable)
	float GetCurrentSpeed();
	
	EAnimalMode GetMode() {
		return mMode;
	}
	UFUNCTION(BlueprintCallable)
	void Test();
private:
	AActor* mPlayer;

	FVector mWanderTarget;
	FVector mWanderDirection;
	EAnimalMode mMode = WANDER;

	EBallMode mBallMode = CATCH;
};
