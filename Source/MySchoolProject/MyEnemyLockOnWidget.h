// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MyEnemyLockOnWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYSCHOOLPROJECT_API UMyEnemyLockOnWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UImage* Image_30;


	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
