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
	static ConstructorHelpers::FClassFinder<UUserWidget> dialogWidgetAsset(TEXT("/Game/Blueprints/UI/DialogUserWidget_BP"));
	static ConstructorHelpers::FClassFinder<UUserWidget> escMenuWidgetAsset(TEXT("/Game/Blueprints/UI/ESCMenuUserWidget_BP"));

	staminaWidgetTemplete = staminaWidgetAsset.Class;
	lockOnWidgetTemplete = lockOnWidgetAsset.Class;
	playerHealthWidgetTemplete = playerHealthWidgetAsset.Class;
	dialogWidgetTemplete = dialogWidgetAsset.Class;
	escMenuWidgetTemplete = escMenuWidgetAsset.Class;

}

void AMyHUD::BeginPlay() {
	Super::BeginPlay();

	playerCont = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	player = Cast<AMyPlayer>(playerCont->GetPawn());

	if (player) {
		//set up stamina and health widget
		staminaWidget = CreateWidget<UMyPlayerStaminaWidget>(GetWorld(), staminaWidgetTemplete);
		staminaWidget->player = player;
		staminaWidget->AddToViewport();

		playerHealthWidget = CreateWidget<UMyPlayerHealthUserWidget>(GetWorld(), playerHealthWidgetTemplete);
		playerHealthWidget->player = player;
		playerHealthWidget->AddToViewport();
	}

	if (playerCont) {
		//when player is locked on enemy spawn the lock on widget
		if (player->lockedOnEnemy) {
			lockOnWidget = CreateWidget<UMyEnemyLockOnWidget>(GetWorld(), lockOnWidgetTemplete);			
			lockOnWidget->AddToViewport();
		}
	}
}

void AMyHUD::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (playerCont != nullptr) {
		if (player != nullptr) {
			if (player->lockedOnEnemy) {
				//if player is locked on enemy spawn the lock on widget
				if (lockOnWidget == nullptr) {
					lockOnWidget = CreateWidget<UMyEnemyLockOnWidget>(GetWorld(), lockOnWidgetTemplete);
				}
			
				if (lockOnWidget != nullptr) {
					if (!lockOnWidget->GetIsVisible()) {
						lockOnWidget->AddToViewport();
					}
				}

			}
			//once player unlocks remove the lock on widget
			else {
				if (lockOnWidget != nullptr && lockOnWidget->GetIsVisible()) {
					lockOnWidget->RemoveFromViewport();
				}
			}
		}
	}


}

void AMyHUD::ManageDialogWidget(bool shouldHide) {

	if (dialogWidget == nullptr) {
		dialogWidget = CreateWidget<UUserWidget>(GetWorld(), dialogWidgetTemplete);
	}

	//hides the dialog widget once player goes too far from the NPC
	if (shouldHide) {
		if (dialogWidget != nullptr) {
			if (dialogWidget->IsInViewport()) {
				dialogWidget->RemoveFromViewport();
			}
		}
	}

	//shows the dialog widget once player gets close enough to the NPC
	else {
		if (dialogWidget != nullptr) {
			if (!dialogWidget->GetIsVisible()) {
				dialogWidget->AddToViewport();
			}
		}
	}

}

void AMyHUD::ManageESCMenu() {
	if (escMenuWidget == nullptr) {
		escMenuWidget = CreateWidget<UUserWidget>(GetWorld(), escMenuWidgetTemplete);
	}

	if (escMenuWidget != nullptr) {
		APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

		//hides ESC menu and hides mouse cursor
		if (escMenuWidget->IsInViewport()) {
			escMenuWidget->RemoveFromViewport();
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(playerController);
			playerController->bShowMouseCursor = false;
			playerController->bEnableClickEvents = false;
			playerController->bEnableMouseOverEvents = false;
			return;
		}

		//shows ESC menu and enables mouse cursor
		if (!escMenuWidget->GetIsVisible()) {
			escMenuWidget->AddToViewport();
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(playerController);
			playerController->bShowMouseCursor = true;
			playerController->bEnableClickEvents = true;
			playerController->bEnableMouseOverEvents = true;
			return;
		}
	}

}