// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "MyPlayerStaminaWidget.generated.h"

/**
 *
 */
class MyPlayer;
UCLASS()
class MYSCHOOLPROJECT_API UMyPlayerStaminaWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMyPlayerStaminaWidget(const FObjectInitializer& ObjectInitializer);

	class AMyPlayer* player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UVerticalBox* VerticalBox_58;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UVerticalBox* VerticalBox_76;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		float maxYellowBarWaitTime{ 1.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		float howFastYellowBarDrains{ 2.0f };

	float currentYellowBarWaitTime{ 0.0f };



	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
