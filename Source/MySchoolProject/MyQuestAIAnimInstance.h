// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyQuestAIAnimInstance.generated.h"

/**
 * 
 */
class MyQuestGiveAI;
UCLASS()
class MYSCHOOLPROJECT_API UMyQuestAIAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool rescued{ false };

	class AMyQuestGiveAI* myQuestAI;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	virtual void BlueprintBeginPlay();
};
