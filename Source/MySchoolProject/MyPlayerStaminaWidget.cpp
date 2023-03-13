// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerStaminaWidget.h"
#include "MyPlayer.h"

UMyPlayerStaminaWidget::UMyPlayerStaminaWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

}

void UMyPlayerStaminaWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	if (VerticalBox_58) {
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(VerticalBox_58->Slot);
		if (slot) {
			if (player) {
				slot->SetSize(FVector2D((player->currentStamina * 500) / player->maxStamina, 10));
			}
		}
	}

	if (VerticalBox_76) {
		UCanvasPanelSlot* yellowSlot = Cast<UCanvasPanelSlot>(VerticalBox_76->Slot);
		UCanvasPanelSlot* greenSlot = Cast<UCanvasPanelSlot>(VerticalBox_58->Slot);

		if (yellowSlot) {
			if (greenSlot) {
				if (player) {
					if (player->isRunning) {
						currentYellowBarWaitTime = maxYellowBarWaitTime;
					}
				}
				if (greenSlot->GetSize().X < yellowSlot->GetSize().X) {
					currentYellowBarWaitTime += InDeltaTime;
				}

				if (currentYellowBarWaitTime >= maxYellowBarWaitTime) {
					yellowSlot->SetSize(FVector2D(yellowSlot->GetSize().X * (1 / howFastYellowBarDrains), 10));
				}

				if (greenSlot->GetSize().X > yellowSlot->GetSize().X) {
					currentYellowBarWaitTime = 0.0f;
					yellowSlot->SetSize(FVector2D(greenSlot->GetSize().X, 10));

				}
			}

		}
	}

}