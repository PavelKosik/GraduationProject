// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEquipmentTileUserWidget.h"
#include "MyPlayer.h"

void UMyEquipmentTileUserWidget::EquipItem(TArray<UStaticMesh*> itemToEquip, TArray<FRotator> rotation, FName type) {
	//Makes player equip item
	Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter())->EquipItem(itemToEquip, rotation, type);

}

void UMyEquipmentTileUserWidget::DeEquipItem(FName type) {
	//Makes player deequip item
	Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter())->DeEquipItem(type);
}