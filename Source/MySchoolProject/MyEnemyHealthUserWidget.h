// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "MyEnemyHealthUserWidget.generated.h"

/**
 * 
 */
class MyEnemyCorpseCharacter;
UCLASS()
class MYSCHOOLPROJECT_API UMyEnemyHealthUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UImage* Image_23;


	UCanvasPanelSlot* slot;
	void ScaleHealthImage(float maxHealth, float currentHealth);
};
