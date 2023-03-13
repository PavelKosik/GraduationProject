// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyCorpseAnimInstance.h"
#include "MyEnemyCorpseCharacter.h"


void UMyEnemyCorpseAnimInstance::BlueprintBeginPlay() {
	Super::BlueprintBeginPlay();
	myCorpse =Cast<AMyEnemyCorpseCharacter>(GetOwningActor());
	
}

void UMyEnemyCorpseAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (myCorpse) {
		isMoving = myCorpse->isMoving;
		isDead = myCorpse->isDead;
		isAttacking = myCorpse->isAttacking;

	}

	else {
		UE_LOG(LogTemp, Warning, TEXT("Corpse failed load"));
		myCorpse = Cast<AMyEnemyCorpseCharacter>(GetOwningActor());
	}
}