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
				slot->SetSize(FVector2D((player->currentPlayerHealth * 500) / player->playerMaxHealth, 15));
			}
		}
	}

	if (VerticalBox_76) {
		UCanvasPanelSlot* yellowSlot = Cast<UCanvasPanelSlot>(VerticalBox_76->Slot);
		UCanvasPanelSlot* redSlot = Cast<UCanvasPanelSlot>(VerticalBox_58->Slot);

		if (yellowSlot) {
			if (redSlot) {
				/*if (player) {
					if (player->isRunning) {
						currentYellowBarWaitTime = maxYellowBarWaitTime;
					}
				}*/
				if (redSlot->GetSize().X < yellowSlot->GetSize().X) {
					currentYellowBarWaitTime += InDeltaTime;
				}

				if (currentYellowBarWaitTime >= maxYellowBarWaitTime) {
					yellowSlot->SetSize(FVector2D(yellowSlot->GetSize().X * (1 / howFastYellowBarDrains), 15));
				}

				if (redSlot->GetSize().X > yellowSlot->GetSize().X) {
					currentYellowBarWaitTime = 0.0f;
					yellowSlot->SetSize(FVector2D(redSlot->GetSize().X, 15));

				}
			}

		}
	}

}
