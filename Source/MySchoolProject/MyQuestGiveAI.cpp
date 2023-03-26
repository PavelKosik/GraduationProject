// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuestGiveAI.h"
#include "MyPlayer.h"
#include "MyHUD.h"
#include "MyEnemyCorpseCharacter.h"

// Sets default values
AMyQuestGiveAI::AMyQuestGiveAI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = mesh;
	dialogStartRange = CreateDefaultSubobject<UStaticMeshComponent>("DialogStartRange");
	dialogStartRange->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyQuestGiveAI::BeginPlay()
{
	Super::BeginPlay();

	AMyQuestGiveAI::BindToInput();

	TInlineComponentArray<UActorComponent*> Components;
	GetComponents(Components);

	for (int i = 0; i < Components.Num(); i++) {

		if (Components[i]->ComponentHasTag("Text")) {
			textComp = Cast<UTextRenderComponent>(Components[i]);
		}

	}

	textComp->SetText(FText::FromString(dialogNotAllRescued[0]));
}

// Called every frame
void AMyQuestGiveAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < enemiesNeededToDie.Num(); i++) {
		if (enemiesNeededToDie[i]->isDead) {
			enemiesNeededToDie.RemoveAt(i);
		}
	}

	if (enemiesNeededToDie.Num() <= 0) {
		if (!rescued) {
			dialogIndex++;
			textComp->SetText(FText::FromString(dialogNotAllRescued[dialogIndex]));
			textComp->SetTextRenderColor(FColor::Yellow);
			rescued = true;

		}

	}

	if (textComp) {
		FVector playerLocation = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter())->cameraAttach->GetComponentLocation();
		FRotator newRot = UKismetMathLibrary::FindLookAtRotation(textComp->GetComponentLocation(), playerLocation);
		textComp->SetWorldRotation(newRot);
	}

	if (rescued) {
		FVector playerLocation = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter())->playerSkeletalMesh->GetComponentLocation();
		FRotator meshRot = UKismetMathLibrary::FindLookAtRotation(mesh->GetComponentLocation(), playerLocation) - FRotator(0, 90, 0);
		mesh->SetWorldRotation(meshRot);

		float distance = FVector::Distance(Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter())->playerSkeletalMesh->GetComponentLocation(), mesh->GetComponentLocation());

		if (distance > maxDistanceDialog) {
			AMyHUD* hud = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AMyHUD>();
			hud->ManageDialogWidget(true);
		}

		else {
			AMyHUD* hud = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AMyHUD>();
			hud->ManageDialogWidget(false);
		}
	}
}

void AMyQuestGiveAI::BindToInput()
{
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("DialogTalk", IE_Pressed, this, &AMyQuestGiveAI::HandleDialog);
		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}

void AMyQuestGiveAI::HandleDialog() {
	if (rescued) {
		if (dialogIndex + 1 < dialogNotAllRescued.Num()) {
			dialogIndex++;
			textComp->SetText(FText::FromString(dialogNotAllRescued[dialogIndex]));
			textComp->SetTextRenderColor(FColor::Black);
		}
	}
}
