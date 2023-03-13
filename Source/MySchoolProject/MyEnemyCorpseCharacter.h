// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"
#include "MyEnemyCorpseCharacter.generated.h"

class MyPlayer;
class MyCorpseEnemyAIController;
class MyEnemyHealthWidgetComponent;
class MyEnemyHealthUserWidget;
UCLASS()
class MYSCHOOLPROJECT_API AMyEnemyCorpseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEnemyCorpseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxHealth{ 400 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float currentHealth{ maxHealth };

	class AMyPlayer* player;

	bool alreadyTookDamageFromAttack{ false };
	bool isMoving{ false };

	bool isDead{ false };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isAttacking{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float attackDamage{ 100.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints", META = (DisplayName = "StaticMeshPlacer"))
		UStaticMeshComponent* staticMeshPlacer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints", META = (Name = "Spawn Waypoint", DisplayAfter = "StaticMeshPlacer"))
		bool shouldSpawnwaypoint{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		TArray<FVector> waypointPositons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoints")
		bool displayWaypoints{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMyEnemyHealthWidgetComponent* enemyHealthWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float timeToHideHealthBarAfterTakingDamage{ 1.2f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float runMoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float steerMoveSpeed;

	class UMyEnemyHealthUserWidget* enemyHealthWidget;
	
	class AMyCorpseEnemyAIController* myCont;
	FTimerHandle showHealthAfterTakenDamageTimerHandle;

	void MyTakeDamage(float damage);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void PostInitializeComponents() override;
	void ShowHealthBar();
	void HideHealthBar();
	UFUNCTION(BlueprintCallable)
		void MyDamagePlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};