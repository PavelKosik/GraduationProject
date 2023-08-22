// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyHealthUserWidget.h"
#include "MyEnemyCorpseCharacter.h"

void UMyEnemyHealthUserWidget::ScaleHealthImage(float maxHealth, float currentHealth) {
	//scales enemy health bar to reflect his current health
	if (slot) {
		slot->SetSize(FVector2D(100 * (currentHealth / maxHealth), 5.0f));
	}

	else {
		slot = Cast<UCanvasPanelSlot>(Image_23->Slot);
	}
}