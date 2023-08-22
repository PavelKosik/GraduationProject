// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerHealthUserWidget.h"
#include "MyPlayer.h"


UMyPlayerHealthUserWidget::UMyPlayerHealthUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

}

void UMyPlayerHealthUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	if (VerticalBox_58) {
		UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(VerticalBox_58->Slot);
		if (slot) {
			if (player) {
				//scales the health bar to reflect the state of the current player health
				slot->SetSize(FVector2D((player->currentPlayerHealth * 500) / player->playerMaxHealth, 15));
			}
		}
	}

	if (VerticalBox_76) {
		//the slot with the health bar showing how much damage player took in last few seconds
		UCanvasPanelSlot* yellowSlot = Cast<UCanvasPanelSlot>(VerticalBox_76->Slot);
		//the slot with the health that reflects the current health state
		UCanvasPanelSlot* redSlot = Cast<UCanvasPanelSlot>(VerticalBox_58->Slot);

		if (yellowSlot) {
			if (redSlot) {
				//if current health bar is smaller size than the yellow one the wait time gets increased
				if (redSlot->GetSize().X < yellowSlot->GetSize().X) {
					currentYellowBarWaitTime += InDeltaTime;
				}

				//once the timer reaches the maximum value the yellow bar starts to shrink to be the same size as the health bar that reflects the current state
				if (currentYellowBarWaitTime >= maxYellowBarWaitTime) {
					yellowSlot->SetSize(FVector2D(yellowSlot->GetSize().X * (1 / howFastYellowBarDrains), 15));
				}

				//once the yellow health bar reaches the size of the other health bar it stops shrinking
				if (redSlot->GetSize().X > yellowSlot->GetSize().X) {
					currentYellowBarWaitTime = 0.0f;
					yellowSlot->SetSize(FVector2D(redSlot->GetSize().X, 15));
				}
			}

		}
	}

}
