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
				//scales the stamina bar to reflect the state of the current player stamina
				slot->SetSize(FVector2D((player->currentStamina * 500) / player->maxStamina, 10));
			}
		}
	}

	if (VerticalBox_76) {
		//the slot with the stamina bar showing how much stamina was used in last few seconds
		UCanvasPanelSlot* yellowSlot = Cast<UCanvasPanelSlot>(VerticalBox_76->Slot);
		//the slot with the stamina that reflects the current stamina state
		UCanvasPanelSlot* greenSlot = Cast<UCanvasPanelSlot>(VerticalBox_58->Slot);

		if (yellowSlot) {
			if (greenSlot) {
				if (player) {
					if (player->isRunning) {
						//there should not be yellow bar visible if player is running
						currentYellowBarWaitTime = maxYellowBarWaitTime;
					}
				}
				//if current stamina bar is smaller size than the yellow one the wait time gets increased
				if (greenSlot->GetSize().X < yellowSlot->GetSize().X) {
					currentYellowBarWaitTime += InDeltaTime;
				}
				//once the timer reaches the maximum value the yellow bar starts to shrink to be the same size as the stamina bar that reflects the current state
				if (currentYellowBarWaitTime >= maxYellowBarWaitTime) {
					yellowSlot->SetSize(FVector2D(yellowSlot->GetSize().X * (1 / howFastYellowBarDrains), 10));
				}

				//once the yellow stamina bar reaches the size of the other stamina bar it stops shrinking
				if (greenSlot->GetSize().X >= yellowSlot->GetSize().X) {
					currentYellowBarWaitTime = 0.0f;
					yellowSlot->SetSize(FVector2D(greenSlot->GetSize().X, 10));

				}
			}

		}
	}

}