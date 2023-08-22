// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "MyPlayerHealthUserWidget.generated.h"

/**
 * 
 */
class MyPlayer;
UCLASS()
class MYSCHOOLPROJECT_API UMyPlayerHealthUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMyPlayerHealthUserWidget(const FObjectInitializer& ObjectInitializer);
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
