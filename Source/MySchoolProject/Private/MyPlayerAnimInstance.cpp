// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerAnimInstance.h"
#include "MyPlayer.h"


void UMyPlayerAnimInstance::BlueprintBeginPlay() {
	Super::BlueprintBeginPlay();
	myPlayer = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UMyPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (myPlayer != nullptr) {
		UMyPlayerAnimInstance::AssingValues();
	}

	else {
		myPlayer = Cast<AMyPlayer>(GetOwningActor());
	}

}

void UMyPlayerAnimInstance::AssingValues() {
	//loads all the states from the player controller to be used for the animation transitions
	isRunning = myPlayer->isRunning;
	isMoving = myPlayer->isMoving;
	isLightAttacking1 = myPlayer->isLightAttacking1;
	isLightAttacking2 = myPlayer->isLightAttacking2;
	isRolling = myPlayer->isRolling;
	isStaggered = myPlayer->isStaggered;
	isDying = myPlayer->isDying;
	isBlocking = myPlayer->isBlocking;


}