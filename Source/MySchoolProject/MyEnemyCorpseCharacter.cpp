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

}

// Called when the game starts or when spawned
void AMyEnemyCorpseCharacter::BeginPlay()
{
	Super::BeginPlay();
	enemyHealthWidget = Cast<UMyEnemyHealthUserWidget>(enemyHealthWidgetComponent->GetUserWidgetObject());
	enemyHealthWidget->SetVisibility(ESlateVisibility::Hidden);
	player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());

}

// Called every frame
void AMyEnemyCorpseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (myCont != nullptr) {
		if (myCont->chasingPlayer || myCont->isAttacking) {
			if (player != nullptr) {
				//when enemy runs after player or is attacking him he should be always facing the player
				FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), player->playerSkeletalMesh->GetComponentLocation() + FVector(0, 0, 125));
				SetActorRotation(lookRotation, ETeleportType::TeleportPhysics);
				
				//between attacks enemy steers around the player
				//when steering enemy is slower
				if (myCont->steeringPlayer) {
					if (movementComponent != nullptr) {
						movementComponent->MaxWalkSpeed = steerMoveSpeed;
					}

					else {
						movementComponent = GetCharacterMovement();
					}
				}
				//if he is not steering he is faster so he can chase and attack player
				else {
					if (movementComponent != nullptr) {
						movementComponent->MaxWalkSpeed = runMoveSpeed;
					}

					else {
						movementComponent = GetCharacterMovement();
					}
				}
			}
		}

	}

	
	if (enemyHealthWidget != nullptr) {
		if (player != nullptr) {
			FVector playerLocation = player->cameraAttach->GetComponentLocation();
			if (FVector::Distance(GetActorLocation(), playerLocation) <= myCont->distanceToSeePlayerAt + 500.0f) {
				if (enemyHealthWidgetComponent != nullptr) {
					//makes sure the enemy health bar is always rotated to be directly looking at the player camera 
					FRotator newRot = UKismetMathLibrary::FindLookAtRotation(enemyHealthWidgetComponent->GetComponentLocation(), playerLocation);
					enemyHealthWidgetComponent->SetWorldRotation(newRot);
					//scales the health bar to reflect current health state
					enemyHealthWidget->ScaleHealthImage(maxHealth, currentHealth);
				}
			}
		}
	}

	if (currentHealth <= 0.0f) {
		//death animation state starts playing
		isDead = true;
		//makes sure that dead enemies can't body block player before they are removed
		GetCapsuleComponent()->SetCapsuleRadius(0.0f);
		if (Cast<AMyEnemyCorpseCharacter>(player->currentlyLockedOnEnemy) == this) {
			//if player was locked on to this enemy it tries to lock on to another one
			player->LockToDifferentEnemyAfterKill();
		}
		//sets timer before enemy is removed
		if (GetWorldTimerManager().IsTimerActive(dieTimerHandle) == false) {
			GetWorld()->GetTimerManager().SetTimer(dieTimerHandle, this, &AMyEnemyCorpseCharacter::Die, timeBeforeDestroy, false);
		}

	}
}

void AMyEnemyCorpseCharacter::MyTakeDamage(float damage) {
	//if enemy takes damage his health bar is shown
	if (enemyHealthWidget != nullptr) {
		enemyHealthWidget->SetVisibility(ESlateVisibility::Visible);
	}

	//decrease the health by the amount passed from player
	currentHealth -= damage;

	//if player is not locked on to this enemy then the health bar is hidden shortly after taking damage
	if ((Cast<AMyEnemyCorpseCharacter>(player->currentlyLockedOnEnemy) == this) == false) {
		GetWorld()->GetTimerManager().SetTimer(showHealthAfterTakenDamageTimerHandle, this, &AMyEnemyCorpseCharacter::HideHealthBar, timeToHideHealthBarAfterTakingDamage, false);
	}
}

void AMyEnemyCorpseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//Gets reference to the controller class
	myCont = Cast<AMyEnemyCorpseAIController>(GetController());
	if (myCont != nullptr) {
		myCont->myEnemyLogic = this;
	}
}

void AMyEnemyCorpseCharacter::ShowHealthBar() {
	//shows the health bar
	if (enemyHealthWidget != nullptr) {
		enemyHealthWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMyEnemyCorpseCharacter::HideHealthBar() {
	//hides the health bar
	if (enemyHealthWidget != nullptr) {
		enemyHealthWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMyEnemyCorpseCharacter::MyDamagePlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (isDead == false) {
		if (myCont != nullptr) {
			if (myCont->playerTookDamageFromCurrentAttack == false) {

				//if enemy weapon collides with player hitbox
				if (OtherComp->ComponentHasTag("PlayerCapsule")) {
					if (player != nullptr) {

						//and player isn't rolling because he shouldn't take damage when he rolls
						if (!player->isRolling) {

							//makes sure player does take damage only once from one animation attack
							myCont->playerTookDamageFromCurrentAttack = true;

							//decreases player health based on his armor
							player->currentPlayerHealth -= attackDamage - player->playerArmorValue;
							
							//staggers player
							player->Stagger();
							return;
						}
					}
				}
			}
		}
	}
}

void AMyEnemyCorpseCharacter::Die() {
	Destroy();
}