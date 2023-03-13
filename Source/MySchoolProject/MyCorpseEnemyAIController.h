// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/CrowdManager.h"
#include "MyCorpseEnemyAIController.generated.h"

/**
 *
 */
class MyPlayer;
class MyEnemyCorpseCharacter;
UCLASS()
class MYSCHOOLPROJECT_API AMyCorpseEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMyCorpseEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float distanceToSeePlayerAt{ 200.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float distanceToStartAttackingAt{ 50.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float attackLength{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float distanceToStopWalkingToPlayerAt{ 50.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxAllowedDistanceFromStartPos{ 1000.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float attackTime{ 1.2f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int numberOfDoneAttacks{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxWaitTimeBetweenAttacks{ 4.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float minWaitTimeBetweenAttacks{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxSteerDistance{ 500.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float minSteerDistance{ 100.0f };

	UCrowdManager* CrowdManager;

	float waitTimeBetweenAttacks{ (minWaitTimeBetweenAttacks + maxWaitTimeBetweenAttacks) / 2.0f };
	FTimerHandle waitTimeBetweenAttacksTimerHandle;
	float currentAttackTime{ 0.0f };

	bool playerTookDamageFromCurrentAttack{ false };
	APawn* myPawn;

	int currentPathIndex{ 0 };

	bool finishedPath{ false };
	UNavigationSystemV1* navMesh;
	class AMyEnemyCorpseCharacter* myEnemyLogic;
	bool chasingPlayer{ false };
	bool shouldReturnToPos{ false };
	FTimerHandle enemyAttackTimerHandle;
	bool steeringPlayer{ false };
	FVector startingPos;

	bool addedPoint{ false };
	bool isAttacking{ false };

	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	void FollowPath();
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void StopAttack();
	void RandomizeWaitTime();
	void SteerAroundPlayer();

	void HandlePlayerTakeDamageTime(float DeltaTime);

};
