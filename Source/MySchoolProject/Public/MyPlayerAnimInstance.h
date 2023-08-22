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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool isMoving{ false };


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool isRunning{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool isLightAttacking1{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool isLightAttacking2{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool isRolling{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool isStaggered{ false };

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		bool isDying{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool isBlocking{ false };
	
	class AMyPlayer* myPlayer;
	//AActor* owner;

	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void BlueprintBeginPlay();
	void AssingValues();
};
