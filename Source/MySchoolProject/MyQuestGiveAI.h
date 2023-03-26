// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyQuestGiveAI.generated.h"

class MyEnemyCorpseCharacter;
class MyPlayer;
class MyHUD;
UCLASS()
class MYSCHOOLPROJECT_API AMyQuestGiveAI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyQuestGiveAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void BindToInput();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class AMyEnemyCorpseCharacter*> enemiesNeededToDie;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> dialogNotAllRescued;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> dialogAllRescued;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* dialogStartRange;

	bool rescued{ false };

	int dialogIndex{ false };

	UTextRenderComponent* textComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxDistanceDialog{ 50.0f };

	void HandleDialog();
};
