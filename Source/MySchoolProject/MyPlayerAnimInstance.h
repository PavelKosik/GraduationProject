// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class MYSCHOOLPROJECT_API UMyPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UMyPlayerAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isMoving{ false };


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isRunning{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isLightAttacking1{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isLightAttacking2{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isRolling{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isStaggered{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isDying{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isBlocking{ false };
	
	class AMyPlayer* myPlayer;
	//AActor* owner;

	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void BlueprintBeginPlay();
	void AssingValues();
};
