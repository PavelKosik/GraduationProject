// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuestGiveAI.h"
#include "MyPlayer.h"
#include "MyHUD.h"
#include "MyQuestAIRescuedManager.h"
#include "MyEnemyCorpseCharacter.h"

// Sets default values
AMyQuestGiveAI::AMyQuestGiveAI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = mesh;
}

// Called when the game starts or when spawned
void AMyQuestGiveAI::BeginPlay()
{
	Super::BeginPlay();

	//AMyQuestGiveAI::BindToInput();

	TInlineComponentArray<UActorComponent*> Components;
	GetComponents(Components);

	for (int i = 0; i < Components.Num(); i++) {

		if (Components[i]->ComponentHasTag("Text")) {
			textComp = Cast<UTextRenderComponent>(Components[i]);
		}
	}
	if (textComp != nullptr) {
		textComp->SetText(FText::FromString(dialogNotAllRescued[0]));
	}
	player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

// Called every frame
void AMyQuestGiveAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//checks if the NPC is rescued
	//the enemies needed to be killed for the  NPC to be rescued are stored in an array
	//the enemy gets removed from the array when they die
	//when they array is empty the NPC is rescued
	for (int i = 0; i < enemiesNeededToDie.Num(); i++) {
		if (enemiesNeededToDie[i] != nullptr) {
			if (enemiesNeededToDie[i]->isDead) {
				enemiesNeededToDie.RemoveAt(i);
			}
		}

		else {
			enemiesNeededToDie.RemoveAt(i);
		}
	}
	//once rescued changes the text to a yellow exclamation mark 
	//that looks like the NPC has quest for the player
	if (enemiesNeededToDie.Num() == 0 || enemiesNeededToDie.IsEmpty()) {
		if (!rescued) {
			dialogIndex++;
			textComp->SetText(FText::FromString(dialogNotAllRescued[dialogIndex]));
			textComp->SetTextRenderColor(FColor::Yellow);
			rescued = true;
		}

	}

	//if player is near enough to see the text
	//the text gets rotated to always face the camera so player can always read the text without problem
	if (textComp) {
		if (player == nullptr) {
			player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
		}
		if (player != nullptr) {
			FVector playerLocation = player->cameraAttach->GetComponentLocation();
			if (FVector::Distance(GetActorLocation(), playerLocation) <= 2000.0f) {
				FRotator newRot = UKismetMathLibrary::FindLookAtRotation(textComp->GetComponentLocation(), playerLocation);
				if (textComp != nullptr) {
					textComp->SetWorldRotation(newRot);
				}
			}
		}
	}
	
	//once rescued the NPC should always face the player
	if (rescued) {
		if (player != nullptr && mesh != nullptr) {
			float distance = FVector::Distance(player->playerSkeletalMesh->GetComponentLocation(), mesh->GetComponentLocation());
			if (distance > 5.0f && distance <= 2000.0f) {
				FVector playerLocation = player->playerSkeletalMesh->GetComponentLocation();
				FRotator meshRot = UKismetMathLibrary::FindLookAtRotation(mesh->GetComponentLocation(), playerLocation) - FRotator(0, 90, 0);
				mesh->SetWorldRotation(meshRot);
			}
		}
	}
}

void AMyQuestGiveAI::Delete() {
	Destroy();
}
