// Fill out your copyright notice in the Description page of Project Settings.
#include "MyHUD.h"
#include "MyPlayer.h"
#include "MyEnemyLockOnWidget.h"
#include "MyPlayerHealthUserWidget.h"
#include "MyPlayerStaminaWidget.h"

AMyHUD::AMyHUD() {
	static ConstructorHelpers::FClassFinder<UMyPlayerStaminaWidget> staminaWidgetAsset(TEXT("/Game/Blueprints/UI/Player/MyStaminaWidget_BP"));
	static ConstructorHelpers::FClassFinder<UMyEnemyLockOnWidget> lockOnWidgetAsset(TEXT("/Game/Blueprints/UI/Player/MyEnemyLockOnWidget_BP"));
	static ConstructorHelpers::FClassFinder<UMyPlayerHealthUserWidget> playerHealthWidgetAsset(TEXT("/Game/Blueprints/UI/Player/MyPlayerHealthUserWidget_BP"));
	//static ConstructorHelpers::FClassFinder<UMyInventoryWidget> inventoryWidgetAsset(TEXT("/Game/Blueprints/UI/MyInventoryWidget_BP"));


	staminaWidgetTemplete = staminaWidgetAsset.Class;
	lockOnWidgetTemplete = lockOnWidgetAsset.Class;
	playerHealthWidgetTemplete = playerHealthWidgetAsset.Class;
	//inventoryWidgetTemplete = inventoryWidgetAsset.Class;

}

void AMyHUD::BeginPlay() {
	Super::BeginPlay();

	playerCont = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	player = Cast<AMyPlayer>(playerCont->GetPawn());

	if (player) {
		staminaWidget = CreateWidget<UMyPlayerStaminaWidget>(GetWorld(), staminaWidgetTemplete);
		staminaWidget->player = player;
		staminaWidget->AddToViewport();

		playerHealthWidget = CreateWidget<UMyPlayerHealthUserWidget>(GetWorld(), playerHealthWidgetTemplete);
		playerHealthWidget->player = player;
		playerHealthWidget->AddToViewport();
		//lockOnWidget = CreateWidget<UMyEnemyLockOnWidget>(GetWorld(), lockOnWidgetTemplete);

	}

	if (playerCont) {
		if (player->lockedOnEnemy) {
			//FVector2D screenPos;
			//if (player->currentlyLockedOnEnemy != nullptr) {
				//playerCont->ProjectWorldLocationToScreen(player->currentlyLockedOnEnemy->GetActorLocation(), screenPos);
				//playerCont->ProjectWorldLocationToScreen(player->currentlyLockedOnEnemy->GetActorLocation(), screenPos, true);

			lockOnWidget = CreateWidget<UMyEnemyLockOnWidget>(GetWorld(), lockOnWidgetTemplete);
			//UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(lockOnWidget->Image_30->Slot);
			//screenPos = screenPos - FVector2D(-250, -250);
			//slot->SetPosition(screenPos);
			lockOnWidget->AddToViewport();
			//}
		}

		/*else {
			lockOnWidget = CreateWidget<UMyEnemyLockOnWidget>(GetWorld(), lockOnWidgetTemplete);
			lockOnWidget->RemoveFromViewport();
		}*/
	}
}

void AMyHUD::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (playerCont) {
		if (player->lockedOnEnemy) {
			//if (player->currentlyLockedOnEnemy != nullptr) {
				//FVector2D screenPos;
				//playerCont->ProjectWorldLocationToScreen(player->currentlyLockedOnEnemy->GetActorLocation(), screenPos, true);

			if (lockOnWidget == nullptr) {
				lockOnWidget = CreateWidget<UMyEnemyLockOnWidget>(GetWorld(), lockOnWidgetTemplete);
			}
			//	screenPos = screenPos - FVector2D(-250, -250);
			//	UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(lockOnWidget->Image_30->Slot);
				//if (slot) {

				//	slot->SetPosition(screenPos);
			if (!lockOnWidget->GetIsVisible()) {
				lockOnWidget->AddToViewport();
			}

			//}
		//}
		}

		else {
			if (lockOnWidget != nullptr) {
				lockOnWidget->RemoveFromViewport();
			}
		}

	/*	if (player->showInventory) {
			if (inventoryWidget == nullptr) {
				inventoryWidget = CreateWidget<UMyInventoryWidget>(GetWorld(), inventoryWidgetTemplete);
			}
			//	screenPos = screenPos - FVector2D(-250, -250);
			//	UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(lockOnWidget->Image_30->Slot);
				//if (slot) {

				//	slot->SetPosition(screenPos);
			if (!inventoryWidget->GetIsVisible()) {
				inventoryWidget->AddToViewport();
			}
		}

		else {
			if (inventoryWidget != nullptr) {
				inventoryWidget->RemoveFromViewport();
			}
		}*/
	}
}