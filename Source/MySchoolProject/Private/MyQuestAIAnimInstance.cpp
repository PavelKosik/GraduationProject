// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuestAIAnimInstance.h"
#include "MyQuestGiveAI.h"

void UMyQuestAIAnimInstance::BlueprintBeginPlay() {
	Super::BlueprintBeginPlay();

	myQuestAI = Cast<AMyQuestGiveAI>(GetOwningActor());
}

void UMyQuestAIAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);
	//the quest NPC has only two animation states
	//which animation is played depends on whether the NPC has been rescued or not
	if (myQuestAI) {
		rescued = myQuestAI->rescued;
	}

	else {
		myQuestAI = Cast<AMyQuestGiveAI>(GetOwningActor());
	}

}