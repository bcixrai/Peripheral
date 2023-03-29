// Fill out your copyright notice in the Description page of Project Settings.


#include "BallGrabComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animal.h"
void UBallGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bCheckForThrow) {
		mCurrentTime += DeltaTime;

		if (mCurrentTime > mCheckForThrowTime) {
			//Find out how fast it moved this 0.1 seconds
			auto dist = FVector::Distance(mReleasedLocation, GetOwner()->GetActorLocation());

			float speed = (dist / 100.f)/ mCurrentTime;

			GEngine->AddOnScreenDebugMessage(113, 10, FColor::Cyan, FString::Printf(TEXT("Ball : %f meters per second"), mCheckForThrowTime, speed));
			bool sufficient = speed > mRequiredMetersPerSecond;
			if (sufficient) {
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::Printf(TEXT("Ball : Registered throw at %f meters per second over %f seconds"), speed, mCheckForThrowTime));
				TArray<AActor*> actors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAnimal::StaticClass(), actors);
				for (auto actor : actors) {
					auto animal = Cast<AAnimal>(actor);
					if (animal) {
						animal->NoticeBall(GetOwner());
					}
				}
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Ball : No throw at %f meters per second over %f seconds"), speed, mCheckForThrowTime));

			}

			//Finished checking
			bCheckForThrow = false;
		}
		
	}
}

void UBallGrabComponent::OnGrabbed()
{
}

void UBallGrabComponent::OnReleased()
{
	//Start timer
	bCheckForThrow = true;
	mCurrentTime = 0;
	mReleasedLocation = GetOwner()->GetActorLocation();
}
