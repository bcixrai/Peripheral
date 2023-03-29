// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Peripheral.h"
#include "PeripheralGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FPeripheralLevel {
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName mLevelName = "AdamMap";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* mThumbnail = nullptr;
};
USTRUCT(BlueprintType)
struct FProfile {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mCortexAPIProfileName = "Adam";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* mPicture = nullptr;
};

UCLASS()
class PERIPHERAL_API UPeripheralGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UPeripheralGameInstance();

	UFUNCTION(BlueprintCallable)
		void SetSelectedMapNameString(FString name) {
		mSelectedMapNameString = name;
	};
	UFUNCTION(BlueprintCallable)
		FString GetSelectedMapNameString() {
		return mSelectedMapNameString;
	};

	class APeripheralHandActor* GetHandByName(FString name) {
		return mHandsMap[name];
	};
	class APeripheralHandActor* GetHandByIndex(int index) {
		return mHandsArray[index];
	};

	void SetHandByName(FString name, APeripheralHandActor* hand) {
		if (mHandsMap.Contains(name)) {
			mHandsMap[name] = hand;
		}
		else {
			mHandsMap.Add({ name, hand });
		}
		
	}
	void SetHandByIndex(int index, APeripheralHandActor* hand) {

	}

	//Levels
	UFUNCTION(BlueprintCallable)
	bool AddLevel(FPeripheralLevel level) {
		if (!mLevels.Contains(level.mLevelName)) {
			mLevels.Add({ level.mLevelName, level });
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("GI : Added level %s"), *level.mLevelName.ToString()));
			return true;
		}
		return false;
	};
	
	UFUNCTION(BlueprintCallable)
	TArray<FPeripheralLevel> GetAllLevelsArray() {
		TArray<FPeripheralLevel> levels;
		for (auto level : mLevels) {
			levels.Add(level.Value);
		}
		return levels;
	};

	bool RequestOpenLevelByName(FName name);

	//Profiles
	UFUNCTION(BlueprintCallable)
	void AddProfile(FProfile profile);
	UFUNCTION(BlueprintCallable)
	TArray<FProfile> GetAllProfiles();
	UFUNCTION(BlueprintCallable)
	FProfile GetProfile(FString name);

	UFUNCTION(BlueprintCallable)
	void SetPeripheralModeFromInstance(EPeripheralMode mode) {
		SetPeripheralMode(mode);
	}
	UFUNCTION(BlueprintCallable)
	EPeripheralMode GetPeripheralModeFromInstance() {
		return GetPeripheralMode();
	}
private:
	
	//Hands
	class APeripheralHandActor* mRightHand;
	APeripheralHandActor* mLeftHand;

	TMap<FString, APeripheralHandActor*> mHandsMap;
	TArray<APeripheralHandActor*> mHandsArray;

	FString mSelectedMapNameString = "M_Lobby";

	TMap<FName, FPeripheralLevel> mLevels;

	TArray<FProfile> mProfiles;
};
