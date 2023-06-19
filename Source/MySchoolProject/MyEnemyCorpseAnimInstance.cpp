// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyCorpseAnimInstance.h"
#include "MyEnemyCorpseCharacter.h"


void UMyEnemyCorpseAnimInstance::BlueprintBeginPlay() {
	Super::BlueprintBeginPlay();
	myCorpse = Cast<AMyEnemyCorpseCharacter>(GetOwningActor());

}

void UMyEnemyCorpseAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	//gets all the values for enemy animation states
	if (myCorpse != nullptr) {
		isMoving = myCorpse->isMoving;
		isDead = myCorpse->isDead;
		isAttacking = myCorpse->isAttacking;

	}

	else {
		myCorpse = Cast<AMyEnemyCorpseCharacter>(GetOwningActor());
	}
}