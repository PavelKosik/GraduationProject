// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "MyHUD.generated.h"

/**
 *
 */
class MyPlayerStaminaWidget;
class MyPlayer;
class MyEnemyLockOnWidget;
class MyPlayerHealthUserWidget;
//class MyInventoryWidget;
UCLASS()
class MYSCHOOLPROJECT_API AMyHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMyHUD();

	TSubclassOf<class UUserWidget> staminaWidgetTemplete;
	TSubclassOf<class UUserWidget> lockOnWidgetTemplete;
	TSubclassOf<class UUserWidget> playerHealthWidgetTemplete;
	TSubclassOf<class UUserWidget> inventoryWidgetTemplete;
	TSubclassOf<class UUserWidget> dialogWidgetTemplete;
	TSubclassOf<class UUserWidget> escMenuWidgetTemplete;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMyPlayerStaminaWidget* staminaWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMyEnemyLockOnWidget* lockOnWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMyPlayerHealthUserWidget* playerHealthWidget;
	//class UMyInventoryWidget* inventoryWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UUserWidget* dialogWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UUserWidget* escMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		APlayerController* playerCont;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AMyPlayer* player;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void ManageDialogWidget(bool shouldHide);
	void ManageESCMenu();


};
