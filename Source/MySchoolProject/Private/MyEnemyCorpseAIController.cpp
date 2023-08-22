// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyCorpseAIController.h"
#include "MyPlayer.h"
#include "MyEnemyCorpseCharacter.h"


AMyEnemyCorpseAIController::AMyEnemyCorpseAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent"))) {

}

void AMyEnemyCorpseAIController::BeginPlay() {
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;
	myPawn = GetPawn();
	startingPos = GetNavAgentLocation();
}

void AMyEnemyCorpseAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (myEnemyLogic != nullptr) {
		if (AttackingNPC && myEnemyLogic->player != nullptr) {
			//enemy attacks the NPC before he meets the player
			//this makes him stop attacking the NPC and 
			//makes him start paying attention to the player once he gets close enough
			if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= distanceToSeePlayerAt) {
				myEnemyLogic->isAttacking = false;
				AttackingNPC = false;
			}
		}

		//each time enemy is walking somewhere make sure it plays the walking animation
		if (IsFollowingAPath() && !myEnemyLogic->isMoving) {
			myEnemyLogic->isMoving = true;
		}
	}


	if (myEnemyLogic != nullptr && myEnemyLogic->player != nullptr) {

		if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= distanceToSeePlayerAt) {
			if (!myEnemyLogic->isAttacking) {
				if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= distanceToStartAttackingAt) {

					//if enemy is near enough to player and can attack him
					if (!GetWorldTimerManager().IsTimerActive(enemyAttackTimerHandle) && !GetWorldTimerManager().IsTimerActive(waitTimeBetweenAttacksTimerHandle)) {

						//plays the attack animation
						myEnemyLogic->isAttacking = true;

						//stops the enemy from moving in the attack animation
						StopMovement();

						//after the animation finishes stops the attack
						GetWorld()->GetTimerManager().SetTimer(enemyAttackTimerHandle, this, &AMyEnemyCorpseAIController::StopAttack, attackLength, false);

						//sets up timer between enemy attacks
						GetWorld()->GetTimerManager().SetTimer(waitTimeBetweenAttacksTimerHandle, this, &AMyEnemyCorpseAIController::RandomizeWaitTime, waitTimeBetweenAttacks, false);
					}

					//if enemy can't attack player because he is waiting for the timer between attacks to run out
					//steering = walking around player
					else {
						AMyEnemyCorpseAIController::SteerAroundPlayer();
					}
				}

				//if enemy is too far from player to attack him
				else {

					//if enemy should attack player he walks towards him
					if (!GetWorldTimerManager().IsTimerActive(waitTimeBetweenAttacksTimerHandle)) {
						chasingPlayer = true;
						MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation());
					}

					//else he just walks around him
					else {
						AMyEnemyCorpseAIController::SteerAroundPlayer();
					}
				}
			}

			//if enemy is attacking make sure he is not moving
			else {
				StopMovement();
			}

			//handles that player doesn't get hit more times from the same attack
			AMyEnemyCorpseAIController::HandlePlayerTakeDamageTime(DeltaTime);

		}


	}
}

void AMyEnemyCorpseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);


	if (myEnemyLogic != nullptr) {
		if (chasingPlayer || myEnemyLogic->isAttacking) {
			return;
		}
	}
}

void AMyEnemyCorpseAIController::StopAttack() {
	//stops the attack
	if (myEnemyLogic != nullptr) {
		myEnemyLogic->isAttacking = false;
	}
}

void AMyEnemyCorpseAIController::RandomizeWaitTime() {
	//enemy waits between his attack so player has time to do stuff
	//the wait duration is random
	waitTimeBetweenAttacks = FMath::RandRange(minWaitTimeBetweenAttacks, maxWaitTimeBetweenAttacks);
}

void AMyEnemyCorpseAIController::SteerAroundPlayer() {
	//if the enemy is currently walking somewhere there is no need to steer
	if (IsFollowingAPath() == false) {
		if (myEnemyLogic != nullptr) {
			if (navMesh != nullptr) {
				steeringPlayer = true;
				if (myEnemyLogic->player != nullptr) {

					//if enemy is too close to the player he walks to opposite direction from the player
					if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= minSteerDistance) {
						StopMovement();
						FVector forwardDirection = myEnemyLogic->player->GetActorLocation() - GetNavAgentLocation();
						forwardDirection = FVector(-forwardDirection.X, -forwardDirection.Y, forwardDirection.Z);
						forwardDirection.Normalize();
						FNavLocation result;
						navMesh->GetRandomPointInNavigableRadius(GetNavAgentLocation() + FVector(forwardDirection.X * 50, forwardDirection.Y * 50, 0), 1.0f, result);
						MoveToLocation(result);
					}

					//if enemy is too far he walks to the player
					if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) >= maxSteerDistance) {
						StopMovement();
						MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation());
					}

					//if enemy is not too far or too close he just walks to a random point around himself
					if (!IsFollowingAPath()) {
						FNavLocation randomLocation;
						navMesh->GetRandomPointInNavigableRadius(GetNavAgentLocation(), 200.0f, randomLocation);
						MoveToLocation(randomLocation);
					}
				}
			}

			else {
				navMesh = UNavigationSystemV1::GetCurrent(this);
				AMyEnemyCorpseAIController::SteerAroundPlayer();
			}

		}
	}
}

void AMyEnemyCorpseAIController::HandlePlayerTakeDamageTime(float DeltaTime) {
	//handles how long the attack animation has been playing
	if (myEnemyLogic != nullptr) {
		if (myEnemyLogic->isAttacking) {
			currentAttackTime += DeltaTime;
		}

		//resets the timer if enemy stops attacking
		else {
			currentAttackTime = 0.0f;
		}

		//if attack time is longer than the animation time
		//the attack timer is reseted and player can take damage from attack again
		//if this wasn't the case player could take damage more than once from the same attacks
		if (currentAttackTime >= attackTime) {
			playerTookDamageFromCurrentAttack = false;
			currentAttackTime = 0.0f;
			numberOfDoneAttacks++;
		}
	}
}