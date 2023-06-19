// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuestAIRescuedManager.h"
#include "MyQuestGiveAI.h"

// Sets default values
AMyQuestAIRescuedManager::AMyQuestAIRescuedManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyQuestAIRescuedManager::BeginPlay()
{
	Super::BeginPlay();
	//makes sure all the quest NPCs have reference to this object
	for (int i = 0; i < notRescuedAI.Num(); i++) {
		if (notRescuedAI[i] != nullptr) {
			notRescuedAI[i]->manager = this;
		}
	}
}

// Called every frame
void AMyQuestAIRescuedManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//tracks all the NPCs that need to be rescued
	//if all but one are rescued then the last NPC's last variable is set to true
	//that makes the last NPC have a special dialog
	for (int i = 0; i < notRescuedAI.Num(); i++) {

		if (notRescuedAI.Num() == 1) {
			if (notRescuedAI[i] != nullptr) {
				notRescuedAI[i]->last = true;
				notRescuedAI[i]->dialogIndex = 0;
			}
		}
		if (notRescuedAI[i] != nullptr) {
			if (notRescuedAI[i]->rescued) {
				notRescuedAI.RemoveAt(i);
			}
		}
	}

}

