// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyCorpseCharacter.h"
#include "MyPlayer.h"
#include "MyEnemyCorpseAIController.h"
#include "MyEnemyHealthUserWidget.h"
#include "MyEnemyHealthWidgetComponent.h"
#include "MyEnemyHealthUserWidget.h"

// Sets default values
AMyEnemyCorpseCharacter::AMyEnemyCorpseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	enemyHealthWidgetComponent = CreateDefaultSubobject<UMyEnemyHealthWidgetComponent>(TEXT("My Enemy Health Widget Component"));
	enemyHealthWidgetComponent->SetupAttachment(RootComponent);
	/*spline = CreateDefaultSubobject<USplineComponent>("Spline");
	spline->SetupAttachment(RootComponent);*/
	//static ConstructorHelpers::FClassFinder<UMyEnemyHealthUserWidget> enemyHealthWidgetAsset(TEXT("/Game/Blueprints/MyEnemyHealthWidget_BP"));
	//enemyHealthWidget = CreateWidget<UMyEnemyHealthUserWidget>(GetWorld(), enemyHealthWidgetAsset);
}

// Called when the game starts or when spawned
void AMyEnemyCorpseCharacter::BeginPlay()
{
	Super::BeginPlay();
	//playerAttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &AMyPlayer::OnOverlapBegin);
	enemyHealthWidget = Cast<UMyEnemyHealthUserWidget>(enemyHealthWidgetComponent->GetUserWidgetObject());
	enemyHealthWidget->SetVisibility(ESlateVisibility::Hidden);
	player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	/*for (int i = 0; i < spline->GetNumberOfSplinePoints(); i++) {
		spline->RemoveSplinePoint(i, true);
	}
	spline->Duration = maxSplineDuration;
	spline->AddSplineLocalPoint(FVector(-25, 0, 0));
	spline->AddSplineLocalPoint(FVector(125, -225, 50));*/
}

// Called every frame
void AMyEnemyCorpseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (myCont) {
		if (myCont->chasingPlayer || myCont->isAttacking) {
			if (player) {
				FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), player->playerSkeletalMesh->GetComponentLocation() + FVector(0, 0, 125));
				SetActorRotation(lookRotation, ETeleportType::TeleportPhysics);

				if (myCont->steeringPlayer) {
					UCharacterMovementComponent* movementComponent = GetCharacterMovement();
					movementComponent->MaxWalkSpeed = steerMoveSpeed;
				}

				else {
					UCharacterMovementComponent* movementComponent = GetCharacterMovement();
					movementComponent->MaxWalkSpeed = runMoveSpeed;
				}
			}


		}

	}
	if (enemyHealthWidget) {
		FVector playerLocation = player->cameraAttach->GetComponentLocation();
		//FVector playerLocation = GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorLocation();
		FRotator newRot = UKismetMathLibrary::FindLookAtRotation(enemyHealthWidgetComponent->GetComponentLocation(), playerLocation);
		enemyHealthWidgetComponent->SetWorldRotation(newRot);
		enemyHealthWidget->ScaleHealthImage(maxHealth, currentHealth);
	}

	if (currentHealth <= 0.0f) {
		isDead = true;
		GetCapsuleComponent()->SetCapsuleRadius(0.0f);
		if (Cast<AMyEnemyCorpseCharacter>(player->currentlyLockedOnEnemy) == this) {
			UE_LOG(LogTemp, Warning, TEXT("LOCK TO DIFFERENT CALLED"));
			player->LockToDifferentEnemyAfterKill();
		}
	}

	/*if (lineStartLocation.Num() > 0) {
		lineStartLocation.Empty();
	}

	if (lineEndLocation.Num() > 0) {
		lineEndLocation.Empty();
	}

	if (steeringPathBlocked.Num() > 0) {
		steeringPathBlocked.Empty();
	}

	if (steeringPathColor.Num() > 0) {
		steeringPathColor.Empty();
	}

	if (possibleDirections.Num() > 0) {
		possibleDirections.Empty();
	}

	for (int i = 0; i < 12; i++) {

		FHitResult hit;
		FCollisionQueryParams collisionParams;

		//collisionParams.AddIgnoredActor(this->GetActor());

		collisionParams.AddIgnoredActor(this);

		FVector currentActorForward = GetActorForwardVector();
		FVector currentAngleDirection = currentActorForward.RotateAngleAxis(i * 30, FVector(0, 0, 1));



		lineStartLocation.Add(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50));

		FVector lineEndLocationToAdd = GetActorLocation() + currentAngleDirection * 300.0f;
		lineEndLocation.Add(FVector(lineEndLocationToAdd.X, lineEndLocationToAdd.Y, lineStartLocation[i].Z));


		bool gotHit = GetWorld()->LineTraceSingleByObjectType(hit, lineStartLocation[i], lineEndLocation[i], ECollisionChannel::ECC_MAX, collisionParams);
		if (gotHit) {
			if (hit.GetActor() == nullptr) {
				if (player) {
					if (hit.GetActor() == player) {
						steeringPathBlocked.Add(true);
					}

					else {
						steeringPathBlocked.Add(false);
					}
				}
			}

		}

		else {
			steeringPathBlocked.Add(false);
		}

	}


	bool lastOneHitPlayer = false;
	bool playerHitAlready = false;

	for (int i = 0; i < steeringPathBlocked.Num(); i++) {

		steeringPathColor.Add(FColor::Orange);
	}

	for (int i = 0; i < steeringPathBlocked.Num(); i++) {
		if (steeringPathBlocked[i] == true) {
			lastOneHitPlayer = true;
			steeringPathColor[i] = FColor::Red;
			if (!playerHitAlready) {
				if (i > 0) {
					possibleDirections.Add(lineEndLocation[i - 1]);
					steeringPathColor[i - 1] = FColor::Green;
					playerHitAlready = true;
				}

				else {
					//UE_LOG(LogTemp, Warning, TEXT("i is equal to zero"));
					possibleDirections.Add(lineEndLocation[steeringPathBlocked.Num() - 1]);
					steeringPathColor[steeringPathBlocked.Num() - 1] = FColor::Green;
					playerHitAlready = true;
				}
			}
		}


		else {
			if (lastOneHitPlayer) {
				possibleDirections.Add(lineEndLocation[i]);
				steeringPathColor[i] = FColor::Green;
				lastOneHitPlayer = false;
			}

		}
	}
}
for (int i = 0; i < steeringPathColor.Num(); i++) {
	DrawDebugLine(GetWorld(), FVector(lineStartLocation[i].X, lineStartLocation[i].Y, lineStartLocation[i].Z), FVector(lineEndLocation[i].X, lineEndLocation[i].Y, lineEndLocation[i].Z), steeringPathColor[i], false, -1, 0, 10);

}
*/



}

/*
void AMyEnemySkeletonCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (player == nullptr) {
		player = Cast<AMyPlayer>(OtherActor);
	}

	if (OtherComp->ComponentHasTag("PlayerAttackHitbox")) {
		if (player->isLightAttacking1 || player->isLightAttacking2) {
		}
	}
}

void AMyEnemySkeletonCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherComp->ComponentHasTag("PlayerAttackHitbox")) {
		//isInAttackHitbox = false;
	}
}
*/
void AMyEnemyCorpseCharacter::MyTakeDamage(float damage) {
	enemyHealthWidget->SetVisibility(ESlateVisibility::Visible);
	currentHealth -= damage;
	if ((Cast<AMyEnemyCorpseCharacter>(player->currentlyLockedOnEnemy) == this) == false) {
		GetWorld()->GetTimerManager().SetTimer(showHealthAfterTakenDamageTimerHandle, this, &AMyEnemyCorpseCharacter::HideHealthBar, timeToHideHealthBarAfterTakingDamage, false);
	}
}

/*void AMyEnemyCharacter::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);

	if (finishedPath==false) {
		if (currentPathIndex + 1 < myEnemyLogic->waypointPositons.Num()) {
			currentPathIndex += 1;
			AMyEnemyCharacter::FollowPath();
			return;
		}

		else {

			finishedPath = true;
			currentPathIndex -= 1;
			return;
		}
	}

	else{
		if (currentPathIndex - 1 >= 0) {
			currentPathIndex -= 1;
			AMyEnemyCharacter::FollowPath();
			return;
		}

		else {

			finishedPath = false;
			currentPathIndex += 1;
			return;
		}
	}
}
*/
/*void AMyEnemyCharacter::FollowPath() {
	if (myPawn) {
		navMesh = UNavigationSystemV1::GetCurrent(this);

		if (navMesh) {
			MoveToLocation(FVector(waypointPositons[currentPathIndex].X, waypointPositons[currentPathIndex].Y, myPawn->GetNavAgentLocation().Z))
		}

		else {
			navMesh = UNavigationSystemV1::GetCurrent(this);
			AMyEnemyCharacter::FollowPath();
		}
	}

	else {
		myPawn = GetPawn();
		AMyEnemyCharacter::FollowPath();
	}
}*/


void AMyEnemyCorpseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	myCont = Cast<AMyEnemyCorpseAIController>(GetController());
	if (myCont) {
		myCont->myEnemyLogic = this;
	}
}

void AMyEnemyCorpseCharacter::ShowHealthBar() {
	enemyHealthWidget->SetVisibility(ESlateVisibility::Visible);

}

void AMyEnemyCorpseCharacter::HideHealthBar() {
	enemyHealthWidget->SetVisibility(ESlateVisibility::Hidden);

}

void AMyEnemyCorpseCharacter::MyDamagePlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//if (OtherActor->ActorHasTag("Player")) {
	if (isDead == false) {
		if (myCont->playerTookDamageFromCurrentAttack == false) {
			if (OtherComp->ComponentHasTag("PlayerCapsule")) {
				if (player) {
					if (!player->isRolling) {
						myCont->playerTookDamageFromCurrentAttack = true;
						player->currentPlayerHealth -= attackDamage;
						player->Stagger();
						return;
					}
				}
			}
		}
	}
}