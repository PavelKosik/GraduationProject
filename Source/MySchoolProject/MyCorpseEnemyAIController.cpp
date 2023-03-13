// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCorpseEnemyAIController.h"
#include "MyPlayer.h"
#include "MyEnemyCorpseCharacter.h"


AMyCorpseEnemyAIController::AMyCorpseEnemyAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent"))) {

}

void AMyCorpseEnemyAIController::BeginPlay() {
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;
	myPawn = GetPawn();
	//navMesh = UNavigationSystemV1::GetCurrent(this);
	startingPos = GetNavAgentLocation();
	AMyCorpseEnemyAIController::FollowPath();

}

void AMyCorpseEnemyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsFollowingAPath()) {
		myEnemyLogic->isMoving = true;
	}

	if (myEnemyLogic) {

		if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= distanceToSeePlayerAt) {
			if (!myEnemyLogic->isAttacking) {
				if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= distanceToStartAttackingAt) {
					if (!GetWorldTimerManager().IsTimerActive(enemyAttackTimerHandle) && !GetWorldTimerManager().IsTimerActive(waitTimeBetweenAttacksTimerHandle)) {
						myEnemyLogic->isAttacking = true;
						StopMovement();
						GetWorld()->GetTimerManager().SetTimer(enemyAttackTimerHandle, this, &AMyCorpseEnemyAIController::StopAttack, attackLength, false);
						GetWorld()->GetTimerManager().SetTimer(waitTimeBetweenAttacksTimerHandle, this, &AMyCorpseEnemyAIController::RandomizeWaitTime, waitTimeBetweenAttacks, false);
					}

					else {
						AMyCorpseEnemyAIController::SteerAroundPlayer();
					}
				}

				else {
					if (!GetWorldTimerManager().IsTimerActive(waitTimeBetweenAttacksTimerHandle)) {
						chasingPlayer = true;
						MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation());
					}

					else {
						AMyCorpseEnemyAIController::SteerAroundPlayer();
					}
				}
			}

			else {
				StopMovement();
				/*
				if (GetWorldTimerManager().IsTimerActive(waitTimeBetweenAttacksTimerHandle)) {
					steeringPlayer = true;
					AMyCorpseEnemyAIController::SteerAroundPlayer();
				}

				else {
					if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) > distanceToStartAttackingAt) {
						chasingPlayer = true;
						//myEnemyLogic->isAttacking = false;
						steeringPlayer = false;
						MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation());
					}
				}*/

			}

			/*else {
				StopMovement();
			}
			*/
		}

		AMyCorpseEnemyAIController::HandlePlayerTakeDamageTime(DeltaTime);

		/*if (!myEnemyLogic->isAttacking) {

			//walk to player when first see him
			if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= distanceToSeePlayerAt) {
				if (steeringPlayer == false) {
					chasingPlayer = true;
					MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation());
					//shouldReturnToPos = true;
				}
			}

			else {
				/*if (shouldReturnToPos) {
					chasingPlayer = false;
					steeringPlayer = false;
					myEnemyLogic->isAttacking = false;
					AMyCorpseEnemyAIController::FollowPath();
				}*/
				/*}
			}
			if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= distanceToStartAttackingAt) {
				if (GetWorldTimerManager().IsTimerActive(enemyAttackTimerHandle) == false) {
					if (GetWorldTimerManager().IsTimerActive(waitTimeBetweenAttacksTimerHandle) == false) {
						myEnemyLogic->isAttacking = true;
						steeringPlayer = false;
						StopMovement();
						GetWorld()->GetTimerManager().SetTimer(waitTimeBetweenAttacksTimerHandle, this, &AMyCorpseEnemyAIController::RandomizeWaitTime, waitTimeBetweenAttacks, false);
						GetWorld()->GetTimerManager().SetTimer(enemyAttackTimerHandle, this, &AMyCorpseEnemyAIController::StopAttack, attackLength, false);
						//myEnemyLogic->spline->AddSplinePoint(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation(), ESplineCoordinateSpace::World, true);
					}

					else {
						if (steeringPlayer == false) {
							StopMovement();
						}
						AMyCorpseEnemyAIController::SteerAroundPlayer();
					}
				}
			}

			else {
				if (chasingPlayer) {
					MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation());
				}
			}

			if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) >= maxSteerDistance && steeringPlayer) {
				StopMovement();
				MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation());
			}

			if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= minSteerDistance && steeringPlayer) {
				StopMovement();
				FVector currentActorForward = myEnemyLogic->player->GetActorLocation();
				//FVector actorBackward = currentActorForward.RotateAngleAxis(320, FVector(0, 0, 1));// currentActorForward + FVector(0, 0, 150);//.RotateAngleAxis(180, FVector(0, 0, 1));
				//actorBackward = actorBackward + GetNavAgentLocation();
				FVector forwardDirection = myEnemyLogic->player->GetActorLocation() - GetNavAgentLocation();
				forwardDirection = FVector(-forwardDirection.X, -forwardDirection.Y, forwardDirection.Z);
				forwardDirection.Normalize();
				//DrawDebugLine(GetWorld(), GetNavAgentLocation() + FVector(0, 0, 150), GetNavAgentLocation() + FVector(forwardDirection.X * 100, forwardDirection.Y* 100, 150), FColor::Red, false, -1, 0, 10);
				FNavLocation result;
				bool aa = navMesh->GetRandomPointInNavigableRadius(GetNavAgentLocation() + FVector(forwardDirection.X * 450, forwardDirection.Y * 450, 0), 1.0f, result);
				FString output = aa ? "true" : "false";
				UE_LOG(LogTemp, Warning, TEXT("MANAGED TO FIND: %s"), *output);
				DrawDebugPoint(GetWorld(), result, 50, FColor::Red, false);

				MoveToLocation(result);
				if (IsFollowingAPath()) {
					UE_LOG(LogTemp, Warning, TEXT("TRUE TRUE TRUE TRUE"));
				}
				//MoveToLocation(actorBackward * 200);
			}
		}



	}

	if (FVector::Distance(GetNavAgentLocation(), startingPos) > maxAllowedDistanceFromStartPos) {
		MoveToLocation(startingPos);
		chasingPlayer = false;
		myEnemyLogic->isAttacking = false;
	}
	//}

	/*else {
		UE_LOG(LogTemp, Warning, TEXT("TRYING TO GET CROWD MANAGER"));
		//CrowdManager = UCrowdManager::GetCurrent(this);
	}*/
	}
}

void AMyCorpseEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);

	/*if (myEnemyLogic) {
		if (!myEnemyLogic->isAttacking) {
			isAttacking = myEnemyLogic->isAttacking;
		}
	}
	*/
	/*if (steeringPlayer) {
		if (GetWorldTimerManager().IsTimerActive(waitTimeBetweenAttacksTimerHandle)) {
			AMyCorpseEnemyAIController::SteerAroundPlayer();
		}

	}*/
	if (chasingPlayer || myEnemyLogic->isAttacking) {
		return;
	}

	if (finishedPath == false) {

		if (currentPathIndex + 1 < myEnemyLogic->waypointPositons.Num()) {
			currentPathIndex += 1;
			AMyCorpseEnemyAIController::FollowPath();
			return;
		}

		else {

			finishedPath = true;
			currentPathIndex -= 1;
			AMyCorpseEnemyAIController::FollowPath();
			return;
		}
	}

	else {
		if (currentPathIndex - 1 >= 0) {
			currentPathIndex -= 1;
			AMyCorpseEnemyAIController::FollowPath();
			return;
		}

		else {

			finishedPath = false;
			currentPathIndex += 1;
			AMyCorpseEnemyAIController::FollowPath();
			return;
		}
	}

	/*if (steeringPlayer == false) {
		if (!chasingPlayer) {
			if (steeringPlayer == false) {
				/*if (shouldReturnToPos) {
					shouldReturnToPos = false;
				}*/
				/*	if (finishedPath == false) {
						if (currentPathIndex + 1 < myEnemyLogic->waypointPositons.Num()) {
							currentPathIndex += 1;
							AMyCorpseEnemyAIController::FollowPath();
							return;
						}

						else {

							finishedPath = true;
							currentPathIndex -= 1;
							AMyCorpseEnemyAIController::FollowPath();
							return;
						}
					}

					else {
						if (currentPathIndex - 1 >= 0) {
							currentPathIndex -= 1;
							AMyCorpseEnemyAIController::FollowPath();
							return;
						}

						else {

							finishedPath = false;
							currentPathIndex += 1;
							AMyCorpseEnemyAIController::FollowPath();
							return;
						}
					}
				}

				/*else {
					if (myEnemyLogic) {
						if (myEnemyLogic->player) {
							if (IsFollowingAPath() == false) {
								//MoveToActor(myEnemyLogic->player, distanceToStopWalkingToPlayerAt, true);
								MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation(), distanceToStopWalkingToPlayerAt, true);
							}
						}
					}

				}*/
				/*	}
				}
			}*/


			/*if (steeringPlayer && GetWorldTimerManager().IsTimerActive(waitTimeBetweenAttacksTimerHandle)) {
				AMyCorpseEnemyAIController::SteerAroundPlayer();
			}*/

}


void AMyCorpseEnemyAIController::FollowPath() {
	if (myPawn) {
		navMesh = UNavigationSystemV1::GetCurrent(this);

		if (navMesh) {
			if (myEnemyLogic->waypointPositons.Num() > 0) {
				MoveToLocation(FVector(myEnemyLogic->waypointPositons[currentPathIndex].X, myEnemyLogic->waypointPositons[currentPathIndex].Y, myPawn->GetNavAgentLocation().Z));
			}
		}

		else {
			navMesh = UNavigationSystemV1::GetCurrent(this);
			AMyCorpseEnemyAIController::FollowPath();
		}
	}

	else {
		myPawn = GetPawn();
		AMyCorpseEnemyAIController::FollowPath();
	}
}

void AMyCorpseEnemyAIController::StopAttack() {
	myEnemyLogic->isAttacking = false;
}

void AMyCorpseEnemyAIController::RandomizeWaitTime() {
	waitTimeBetweenAttacks = FMath::RandRange(minWaitTimeBetweenAttacks, maxWaitTimeBetweenAttacks);
}

void AMyCorpseEnemyAIController::SteerAroundPlayer() {
	if (IsFollowingAPath() == false) {
		if (myEnemyLogic) {
			//if (myEnemyLogic->possibleDirections.Num() > 0) {
			if (navMesh) {
				//	if (CrowdManager) {
				UE_LOG(LogTemp, Warning, TEXT("Steering around player"));

				steeringPlayer = true;

				if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) <= minSteerDistance) {
					StopMovement();
					FVector forwardDirection = myEnemyLogic->player->GetActorLocation() - GetNavAgentLocation();
					forwardDirection = FVector(-forwardDirection.X, -forwardDirection.Y, forwardDirection.Z);
					forwardDirection.Normalize();
					FNavLocation result;
					navMesh->GetRandomPointInNavigableRadius(GetNavAgentLocation() + FVector(forwardDirection.X * 50, forwardDirection.Y * 50, 0), 1.0f, result);
					MoveToLocation(result);
				}

				if (FVector::Distance(GetNavAgentLocation(), myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation()) >= maxSteerDistance) {
					StopMovement();
					MoveToLocation(myEnemyLogic->player->playerSkeletalMesh->GetComponentLocation());
				}

				if (!IsFollowingAPath()) {
					FNavLocation randomLocation;
					navMesh->GetRandomPointInNavigableRadius(GetNavAgentLocation(), 200.0f, randomLocation);
					MoveToLocation(randomLocation);
				}


			}

			else {
				navMesh = UNavigationSystemV1::GetCurrent(this);
				AMyCorpseEnemyAIController::SteerAroundPlayer();
			}

		}
	}
}

void AMyCorpseEnemyAIController::HandlePlayerTakeDamageTime(float DeltaTime) {
	if (myEnemyLogic->isAttacking) {
		currentAttackTime += DeltaTime;
	}

	else {
		currentAttackTime = 0.0f;
	}

	if (currentAttackTime >= attackTime) {
		playerTookDamageFromCurrentAttack = false;
		currentAttackTime = 0.0f;
		numberOfDoneAttacks++;
	}
}