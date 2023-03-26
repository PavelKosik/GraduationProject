// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuestAIAnimInstance.h"
#include "MyQuestGiveAI.h"

void UMyQuestAIAnimInstance::BlueprintBeginPlay() {
	Super::BlueprintBeginPlay();

	myQuestAI = Cast<AMyQuestGiveAI>(GetOwningActor());
}

void UMyQuestAIAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (myQuestAI) {
		rescued = myQuestAI->rescued;
	}

	else {
		myQuestAI = Cast<AMyQuestGiveAI>(GetOwningActor());
	}

}