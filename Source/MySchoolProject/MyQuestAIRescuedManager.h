// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyQuestAIRescuedManager.generated.h"

class MyQuestGiveAI;
UCLASS()
class MYSCHOOLPROJECT_API AMyQuestAIRescuedManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyQuestAIRescuedManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class AMyQuestGiveAI*> notRescuedAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool giveReward{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool gaveReward{ false };
};
