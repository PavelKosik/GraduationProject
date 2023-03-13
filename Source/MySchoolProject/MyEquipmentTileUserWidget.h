// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StaticMesh.h"
#include "MyEquipmentTileUserWidget.generated.h"

/**
 *
 */
class MyPlayer;
UCLASS()
class MYSCHOOLPROJECT_API UMyEquipmentTileUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		void EquipItem(TArray<UStaticMesh*> itemToEquip, TArray<FRotator> rotation, FName type);

	UFUNCTION(BlueprintCallable)
		void DeEquipItem(FName type);
};
