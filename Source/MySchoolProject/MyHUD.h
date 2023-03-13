// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
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


	class UMyPlayerStaminaWidget* staminaWidget;
	class UMyEnemyLockOnWidget* lockOnWidget;
	class UMyPlayerHealthUserWidget* playerHealthWidget;
	//class UMyInventoryWidget* inventoryWidget;

	APlayerController* playerCont;
	class AMyPlayer* player;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;



};