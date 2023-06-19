// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyEnemyCorpseAnimInstance.generated.h"

/**
 *
 */

class MyEnemyCorpseCharacter;
UCLASS()
class MYSCHOOLPROJECT_API UMyEnemyCorpseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isMoving{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isDead{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isAttacking{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AMyEnemyCorpseCharacter* myCorpse;


	virtual void BlueprintBeginPlay();
	virtual void NativeUpdateAnimation(float DeltaSeconds);

};
