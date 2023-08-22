// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayer.h"
#include "MyEnemyCorpseCharacter.h"
#include "MyQuestAIRescuedManager.h"
#include "MyQuestGiveAI.h"
#include "MyHUD.h"

AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	playerSkeletalMesh = GetMesh();
	capsule = GetCapsuleComponent();
	movementComponent = GetCharacterMovement();

	RootComponent = capsule;
	playerAttackHitbox = CreateDefaultSubobject<UStaticMeshComponent>("PlayerAttackHitbox");

	playerDialogHitbox = CreateDefaultSubobject<UStaticMeshComponent>("PlayerDialogHitbox");

	cameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	cameraAttach = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	playerSkeletalMesh->SetupAttachment(RootComponent);
	playerAttackHitbox->SetupAttachment(playerSkeletalMesh);
	playerDialogHitbox->SetupAttachment(playerSkeletalMesh);
	cameraAttach->SetupAttachment(RootComponent);
	cameraComponent->SetupAttachment(cameraAttach, USpringArmComponent::SocketName);
	cameraAttach->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 95.0f), FRotator(0.0f, -5.0f, 0.0f));
	cameraAttach->TargetArmLength = 325.0f;
	cameraAttach->bEnableCameraLag = false;

}

// Called when the game starts or when spawned
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();
	playerAttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &AMyPlayer::OnOverlapBegin);
	playerAttackHitbox->OnComponentEndOverlap.AddDynamic(this, &AMyPlayer::OnOverlapEnd);

	playerDialogHitbox->OnComponentBeginOverlap.AddDynamic(this, &AMyPlayer::OnOverlapBeginDialog);
	playerDialogHitbox->OnComponentEndOverlap.AddDynamic(this, &AMyPlayer::OnOverlapEndDialog);

	hud = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AMyHUD>();

	TInlineComponentArray<UActorComponent*> Components;
	GetComponents(Components);

	for (int i = 0; i < Components.Num(); i++) {

		if (Components[i]->ComponentHasTag("PlayerSword")) {
			playerSwordHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("PlayerShield")) {
			playerShieldHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("PlayerArmor")) {
			playerArmorHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("BootL")) {
			bootLHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("BootR")) {
			bootRHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("ShoulderR")) {
			shoulderRHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("ShoulderL")) {
			shoulderLHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("GloveR")) {
			gloveRHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("GloveL")) {
			gloveLHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("Pants")) {
			pantsHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("Belt")) {
			beltHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("ThighArmorR")) {
			thighArmorRHolder = Cast<UStaticMeshComponent>(Components[i]);
		}

		if (Components[i]->ComponentHasTag("ThighArmorL")) {
			thighArmorLHolder = Cast<UStaticMeshComponent>(Components[i]);
		}
	}

}

// Called every frame
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isStaggered) {

		deltaTimeForRolling = DeltaTime;
		isMoving = false;

		AMyPlayer::HandleStamina(DeltaTime);
		AMyPlayer::HandleRolling();
		AMyPlayer::HandleAttacking(DeltaTime);
		AMyPlayer::HandleCamera();

		if (isRunning && currentStamina <= 0.0f) {
			isRunning = false;
		}

		AMyPlayer::HandleDeath();
	}

	if (horizontalForRoll == 0 && verticalForRoll == 0 && currentStaminaDrain == sprintStaminaDrain) {
		currentStaminaDrain = 0.0f;
	}

	for (int i = 0; i < dialogAI.Num(); i++) {
		if (dialogAI[i] != nullptr) {
			if (dialogAI[i]->rescued) {

				if (hud == nullptr) {
					hud = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AMyHUD>();
				}
				else {
					if (hud->dialogWidget == nullptr || !hud->dialogWidget->GetIsVisible()) {
						hud->ManageDialogWidget(false);
					}
				}
			}
		}

		else {
			dialogAI.RemoveAt(i);
		}
	}


}



// Called to bind functionality to input
void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Horizontal", this, &AMyPlayer::MoveHorizontal);
	PlayerInputComponent->BindAxis("Vertical", this, &AMyPlayer::MoveVertical);
	PlayerInputComponent->BindAxis("RotateX", this, &AMyPlayer::CameraHorizontal);
	PlayerInputComponent->BindAxis("RotateY", this, &AMyPlayer::CameraVertical);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMyPlayer::StartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMyPlayer::StopRun);
	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AMyPlayer::LightAttack);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &AMyPlayer::StartRoll);
	PlayerInputComponent->BindAction("LockOnEnemy", IE_Pressed, this, &AMyPlayer::LockOnEnemy);
	PlayerInputComponent->BindAction("DialogTalk", IE_Pressed, this, &AMyPlayer::HandleDialog);
	PlayerInputComponent->BindAction("ESCMenu", IE_Pressed, this, &AMyPlayer::ESCMenuManage);

}

void AMyPlayer::MoveHorizontal(float horizontal) {
	//player can only move if he isn't staggered by the enemies
	if (!isStaggered) {
		//when in roll player is locked to move only in the roll direction until the roll is done
		if (!isRolling) {
			//player is locked from moving when he is attacking
			if (!isLightAttacking1 && !isLightAttacking2) {
				//stores the last horizontal input in case player would decide to roll
				horizontalForRoll = horizontal;
				//player's movement is based on where the camera is looking
				FRotator rightRotation = cameraAttach->GetComponentRotation();
				//we only want to rotate the player on the Yaw axis
				//because other axis could cause weird rotations occurring
				FRotator yawRotation(0.0f, rightRotation.Yaw, 0.0f);
				//direction based on the current camera rotation
				FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

				if (isRunning) {
					if (horizontal != 0) {
						//lets the stamina system know that player is currently running
						currentStaminaDrain = sprintStaminaDrain;
						//sets speed to runningSpeed so player walks faster
						movementComponent->MaxWalkSpeed = runningSpeed;
					}
				}

				else {
					//if player is not running then the walkSpeed is set to normal walk speed
					movementComponent->MaxWalkSpeed = playerMovespeed;
				}

				//makes player move in the rotation direction based on horizontal (input from player - A or D)
				AddMovementInput(rightDirection, horizontal);

				if (horizontal != 0) {
					//isMoving is variable used in animations
					//setting it to true makes player enter moving state in animation
					isMoving = true;
					//rotates the player model to the direction he is headed
					AMyPlayer::RotateToMovementHorizontal(horizontal);

				}
			}
		}
	}
}

void AMyPlayer::MoveVertical(float vertical) {
	//player can only move if he isn't staggered by the enemies
	if (!isStaggered) {
		//when in roll player is locked to move only in the roll direction until the roll is done
		if (!isRolling) {
			//player is locked from moving when he is attacking
			if (!isLightAttacking1 && !isLightAttacking2) {
				//stores the last vertical input in case player would decide to roll
				verticalForRoll = vertical;
				//player's movement is based on where the camera is looking
				FRotator forwardRotation = cameraAttach->GetComponentRotation();
				//we only want to rotate the player on the Yaw axis
				//because other axis could cause weird rotations occurring
				FRotator yawRotation(0.0f, forwardRotation.Yaw, 0.0f);
				//direction based on the current camera rotation
				FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);


				if (isRunning) {
					if (vertical != 0) {
						//lets the stamina system know that player is currently running
						currentStaminaDrain = sprintStaminaDrain;
						//sets speed to runningSpeed so player walks faster
						movementComponent->MaxWalkSpeed = runningSpeed;
					}
				}


				else {
					//if player is not running then the walkSpeed is set to normal walk speed
					movementComponent->MaxWalkSpeed = playerMovespeed;
				}
				//makes player move in the rotation direction based on vertical (input from player - A or D)
				AddMovementInput(forwardDirection, vertical);

				if (vertical != 0) {
					//isMoving is variable used in animations
					//setting it to true makes player enter moving state in animation
					isMoving = true;
					//rotates the player model to the direction he is headed
					AMyPlayer::RotateToMovementVertical(vertical);

				}
			}
		}
	}

}

void AMyPlayer::CameraHorizontal(float horizontal) {
	if (!lockedOnEnemy) {
		cameraAttach->AddLocalRotation(FRotator(0, cameraRotationSpeed * horizontal, 0));
	}
}

void AMyPlayer::CameraVertical(float vertical) {
	if (!lockedOnEnemy) {
		cameraAttach->AddLocalRotation(FRotator(cameraRotationSpeed * vertical * invertedMouse, 0, 0));
	}
}

void AMyPlayer::RotateToMovementVertical(float vertical) {
	//player model should rotate based on camera only when player isn't locked on enemy
	//because if player is locked on the enemy then the player model is facing the enemy
	if (!lockedOnEnemy) {
		if (vertical > 0) {
			//smoothly rotates player mesh from his current rotation towards the camera rotation
			playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 90, 0), 0.03f, 10.0f));
		}

		if (vertical < 0) {
			//smoothly rotates player mesh from his current rotation towards the camera rotation
			playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 270, 0), 0.03f, 10.0f));
		}
	}
}


void AMyPlayer::RotateToMovementHorizontal(float horizontal) {
	//player model should rotate based on camera only when player isn't locked on enemy
	//because if player is locked on the enemy then the player model is facing the enemy
	if (!lockedOnEnemy) {

		if (horizontal < 0) {
			//smoothly rotates player mesh from his current rotation towards the camera rotation
			playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 180, 0), 0.03f, 10.0f));
		}

		if (horizontal > 0)
		{
			//smoothly rotates player mesh from his current rotation towards the camera rotation
			playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 360, 0), 0.03f, 10.0f));
		}
	}
}

void AMyPlayer::StartRun() {
	//player can only run if he has at least some stamina left
	if (currentStamina > 0.0f) {
		//enables the running animation
		isRunning = true;
	}
}

//called once player lets go of the SHIFT key or his stamina reaches 0
void AMyPlayer::StopRun() {
	//disables the running animation
	isRunning = false;
}

void AMyPlayer::LightAttack() {
	//player isn't allowed to attack when he is staggered
	if (!isStaggered) {
		//attack can only be used if player has enough stamina
		if (currentStamina >= lightAttackStaminaDrain) {
			if (isLightAttacking1 == false) {
				if (isLightAttacking2) {
					//properly removes the stamina if player spams the attack button
					shouldRemoveStaminaWhenTheNextAttackStarts = true;
				}
				else {
					//important to set because it resets player stamina restore time
					shouldRemoveStaminaAfterLightAttack = true;
				}

				isLightAttacking1 = true;

				//makes the attack come through
				if (GetWorldTimerManager().IsTimerActive(lightAttackTimerHandle) == false) {
					GetWorld()->GetTimerManager().SetTimer(lightAttackTimerHandle, this, &AMyPlayer::DoLightAttack, lightAttack1AnimLenght, false);

				}

				else {
					GetWorld()->GetTimerManager().ClearTimer(lightAttackTimerHandle);
					GetWorld()->GetTimerManager().SetTimer(lightAttackTimerHandle, this, &AMyPlayer::DoLightAttack, lightAttack1AnimLenght, false);
				}
			}

			else {
				//sequences another attack if player spams the attack button
				if (isLightAttacking2 == false) {

					shouldRemoveStaminaWhenTheNextAttackStarts = true;

					isLightAttacking2 = true;

					if (GetWorldTimerManager().IsTimerActive(lightAttackTimerHandle) == false) {
						GetWorld()->GetTimerManager().SetTimer(lightAttackTimerHandle, this, &AMyPlayer::DoLightAttack, lightAttack2AnimLenght, false);

					}

					else {
						GetWorld()->GetTimerManager().ClearTimer(lightAttackTimerHandle);
						GetWorld()->GetTimerManager().SetTimer(lightAttackTimerHandle, this, &AMyPlayer::DoLightAttack, lightAttack2AnimLenght, false);
					}
				}
			}
		}
	}
}

void AMyPlayer::DoLightAttack() {
	//allows animation transition between attacks
	isLightAttacking1 = false;
	isLightAttacking2 = false;

}

void AMyPlayer::HandleStamina(float DeltaTime) {
	//when player atttacks his stamina is removed and the recovery time is reseted
	if (shouldRemoveStaminaAfterLightAttack) {
		currentStaminaRecoveryTime = 0.0f;
		currentStamina -= lightAttackStaminaDrain;
		//makes sure that player stamina is only removed the frame player attacked in
		shouldRemoveStaminaAfterLightAttack = false;
	}
	//curentStaminaDrain is set depending on what the player is doing in the adequate functions
	currentStamina -= currentStaminaDrain;

	//player can run only when he is not doing other stuff that would cause him to lose stamina
	//this makes sure that once player lets go off the run button he stop losing stamina
	if (!isRunning) {
		currentStaminaDrain = 0.0f;
	}

	//if stamina hasn't been drained the recovery time is increased
	if (currentStaminaDrain == 0.0f) {
		currentStaminaRecoveryTime += DeltaTime;
	}
	//else it's reseted
	else {
		currentStaminaRecoveryTime = 0.0f;
	}
	//if the stamina hasn't been drained for sufficent amount the stamina starts to increase
	if (currentStaminaRecoveryTime >= timeBeforeStaminaRecovery) {
		currentStamina += staminaRecoverySpeed;
	}
	//in case stamina would go to negative
	if (currentStamina < 0.0f) {
		currentStamina = 0.0f;
	}
	//in case stamina would regen above the maximum amount
	if (currentStamina > maxStamina) {
		currentStamina = maxStamina;
	}

}

void AMyPlayer::StartRoll() {
	//player can only roll when he isn't attacking
	if (!isLightAttacking1 && !isLightAttacking2) {
		//prevents player to spam the roll
		if (!isRolling) {
			//player can only roll if he has enough stamina
			if (currentStamina >= rollStaminaDrain) {
				//makes player enter roll animation state
				isRolling = true;
				//player rolls have a certain duration which is handled by currentRollTime and rollAnimDuration
				currentRollTime = 0.0f;
				//decreases the stamina by rollStaminaDrain when player starts the roll
				currentStamina -= rollStaminaDrain;
				//resets the player stamina recovery time
				currentStaminaRecoveryTime = 0.0f;
				//makes player move in the roll direction
				HandleRolling();
			}
		}
	}
}

void AMyPlayer::HandleRolling() {
	//player should only be moving roll like when he is in roll
	if (isRolling) {

		FRotator rotation = playerSkeletalMesh->GetComponentRotation();
		FRotator yawRotation(0, rotation.Yaw, 0);
		FVector direction = FVector(0, 0, 0);
		//if the player is locked on enemy he rolls relative to him and input
		if (lockedOnEnemy) {

			if (horizontalForRoll != 0) {
				direction += FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X) * -horizontalForRoll;
			}

			if (verticalForRoll != 0) {
				direction += FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y) * verticalForRoll;
			}
		}
		//if player is not locked he rolls the direction he is currently facing
		if (direction.IsZero()) {
			direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		}
		//makes player move in the direction of the roll
		movementComponent->MaxWalkSpeed = rollSpeed;
		AddMovementInput(direction, 1);
		//increases the time that player is rolling
		currentRollTime += deltaTimeForRolling;

	}
	//stop the player roll once he has finished rolling
	if (currentRollTime >= rollAnimDuration) {
		isRolling = false;
	}
}

void AMyPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	//hitbox is done with a cube called attackHitbox that can overlap enemies
	//if player decides to attack all the enemies who are currently overlapping the hitbox take damage
	if (Cast<AMyEnemyCorpseCharacter>(OtherActor) != nullptr) {
		//each enemy that overlaps is added to the array
		//so he can take the damage
		overlappingEnemies.AddUnique(Cast<AMyEnemyCorpseCharacter>(OtherActor));
	}
}

void AMyPlayer::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if the enemy stops overlapping the attackHitbox
	if (Cast<AMyEnemyCorpseCharacter>(OtherActor) != nullptr) {
		//each enemy that stops overlapping is removed from the array
		//so he doesn't take damage anymore
		overlappingEnemies.Remove(Cast<AMyEnemyCorpseCharacter>(OtherActor));
	}
}

void AMyPlayer::LockOnEnemy() {
	//by default there is no enemy to lock onto
	bool foundEnemyToLockOn = false;
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	//all enemies are of type APawn
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> ignoreActors;
	//player actor is ignored because else player would lock on to himself
	ignoreActors.Init(this, 1);
	AActor* enemyToLockOn = nullptr;

	TArray<AActor*> outActors;
	//sphere overlap all actors in the direction camera is currently facing 
	UKismetSystemLibrary::SphereOverlapActors(GetOwner(), playerSkeletalMesh->GetComponentLocation(), maxTargetLockDistance, traceObjectTypes, AMyEnemyCorpseCharacter::StaticClass(), ignoreActors, outActors);

	//in case player was previously locked on any other enemy
	if (lockedOnEnemy) {
		class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
		if (enemy) {
			//when player locks on to enemy his healthbar is shown
			//player being already locked on enemy and pressing the lock button again causes player to unlock
			//the enemy health bar gets hidden because of that
			enemy->HideHealthBar();
		}
		//player gets unlocked and the reference to the lockedEnemy is set to nullptr;
		lockedOnEnemy = false;
		currentlyLockedOnEnemy = nullptr;
		foundEnemyToLockOn = false;
		return;
	}

	else {
		float closestDotToCenter = 0.f;
		//search through all the overlapped enemies
		for (int i = 0; i < outActors.Num(); i++) {
			float dot = FVector::DotProduct(cameraAttach->GetForwardVector(), (outActors[i]->GetActorLocation() - playerSkeletalMesh->GetComponentLocation()).GetSafeNormal());
			//the cast with the if statement makes sure that all the objects to lock onto are enemies
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(outActors[i]);
			if (enemy) {
				//player can only lock onto enemies which aren't dead
				if (!enemy->isDead) {
					//if earlier in the loop there was enemy found
					if (enemyToLockOn != nullptr) {
						//calculates distance to the old enemy and distance to the new enemy
						float distanceOld = FVector::Distance(enemyToLockOn->GetActorLocation(), GetActorLocation());
						float distanceNew = FVector::Distance(outActors[i]->GetActorLocation(), GetActorLocation());
						//if the new enemy is closer to player he gets set as the new enemy to lock on
						if (distanceOld > distanceNew) {
							closestDotToCenter = dot;
							foundEnemyToLockOn = true;
							enemyToLockOn = outActors[i];
						}
					}
					//if there was no enemy to lock on to yet, the enemy gets selected no matter the distance
					else {
						closestDotToCenter = dot;
						foundEnemyToLockOn = true;
						enemyToLockOn = outActors[i];
					}
				}
			}
		}
	}


	//if there has been found enemy in the player view
	if (enemyToLockOn != nullptr) {
		if (currentlyLockedOnEnemy != enemyToLockOn) {
			currentlyLockedOnEnemy = enemyToLockOn;
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
			if (enemy) {
				//enemies healthbar is shown
				enemy->ShowHealthBar();
			}
		}
	}

	//if the enemy to lock on has been found
	//still makes player camera look at the enemy until he unlocks
	if (foundEnemyToLockOn) {
		lockedOnEnemy = true;
	}

	//if no enemy to lock on has been found
	else {
		//makes the camera look the direction the player model is currently facing
		cameraAttach->SetWorldRotation(FRotator(FMath::Clamp(cameraAttach->GetComponentRotation().Pitch, -55.0f, 55.0f), playerSkeletalMesh->GetComponentRotation().Yaw + 90.0f, 0));
	}
}

//called if player kills the enemy he was locked on
//the same search like in LockOnEnemy function repeats itself to see if there are any more enemies nearby
//if there are the player gets locked on to the closest one
//else he gets unlocked
void AMyPlayer::LockToDifferentEnemyAfterKill() {
	
	bool foundEnemyToLockOn = false;
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> ignoreActors;

	ignoreActors.Init(this, 1);
	AActor* enemyToLockOn = nullptr;

	TArray<AActor*> outActors;
	UKismetSystemLibrary::SphereOverlapActors(GetOwner(), playerSkeletalMesh->GetComponentLocation(), maxTargetLockDistance, traceObjectTypes, AMyEnemyCorpseCharacter::StaticClass(), ignoreActors, outActors);

	float closestDotToCenter = 0.f;
	for (int i = 0; i < outActors.Num(); i++) {
		float dot = FVector::DotProduct(cameraAttach->GetForwardVector(), (outActors[i]->GetActorLocation() - playerSkeletalMesh->GetComponentLocation()).GetSafeNormal());

		class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(outActors[i]);

		if (enemy) {
			if (!enemy->isDead) {
				if (enemyToLockOn != nullptr) {
					float distanceOld = FVector::Distance(enemyToLockOn->GetActorLocation(), GetActorLocation());
					float distanceNew = FVector::Distance(outActors[i]->GetActorLocation(), GetActorLocation());
					if (distanceOld > distanceNew) {
						closestDotToCenter = dot;
						foundEnemyToLockOn = true;
						enemyToLockOn = outActors[i];
					}
				}

				else {
					closestDotToCenter = dot;
					foundEnemyToLockOn = true;
					enemyToLockOn = outActors[i];
				}
			}
		}

	}

	if (enemyToLockOn != nullptr) {
		if (currentlyLockedOnEnemy != enemyToLockOn) {
			currentlyLockedOnEnemy = enemyToLockOn;
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
			if (enemy) {
				enemy->ShowHealthBar();
			}
		}

		else {
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
			if (enemy) {
				enemy->HideHealthBar();
			}
			foundEnemyToLockOn = false;
			lockedOnEnemy = false;
			currentlyLockedOnEnemy = nullptr;

		}
	}

	else {
		if (currentlyLockedOnEnemy != nullptr) {
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
			if (enemy) {
				enemy->HideHealthBar();
			}
		}
		foundEnemyToLockOn = false;
		lockedOnEnemy = false;
		currentlyLockedOnEnemy = nullptr;
	}

}
//gets called when player takes damage from enemies
void AMyPlayer::Stagger() {
	//makes sure the player stagger animation state is played
	isStaggered = true;
	//when this timer is active player can't move, roll or attack
	GetWorld()->GetTimerManager().SetTimer(staggerTimerHandle, this, &AMyPlayer::StopStagger, staggerDuration, false);

}

//called once the timer set in Stagger function runs out
void AMyPlayer::StopStagger() {
	//changes animation and allows player to play again
	isStaggered = false;
}

//this gets called a few seconds after player was killed
void AMyPlayer::RestartLevelAfterPlayerDie() {
	//the level is reloaded after the player gets killed
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


//this function makes player be able to equip items
//it searches through all the player items slots to see where it should equip the item
//after it finds the slot it changes the slots mesh and gives player the stats from the item
void AMyPlayer::EquipItem(TArray<UStaticMesh*> itemToEquip, TArray<FRotator> rotation, FName type) {

	if (type == "Sword") {
		playerSwordHolder->SetRelativeRotation(rotation[0]);
		playerSwordHolder->SetStaticMesh(itemToEquip[0]);
		lightAttackDamage = lightAttackDamage * 3.0f;
	}

	if (type == "Shield") {
		playerShieldHolder->SetRelativeRotation(rotation[0]);
		playerShieldHolder->SetStaticMesh(itemToEquip[0]);
		playerArmorValue += 5;
	}

	if (type == "Armor") {
		playerArmorHolder->SetRelativeRotation(rotation[0]);
		playerArmorHolder->SetStaticMesh(itemToEquip[0]);
		playerArmorValue += 5;

	}

	if (type == "Boots") {
		bootLHolder->SetRelativeRotation(rotation[0]);
		bootLHolder->SetStaticMesh(itemToEquip[0]);
		bootRHolder->SetRelativeRotation(rotation[1]);
		bootRHolder->SetStaticMesh(itemToEquip[1]);
		playerSkeletalMesh->HideBoneByName("calf_r", EPhysBodyOp::PBO_None);
		playerSkeletalMesh->HideBoneByName("calf_l", EPhysBodyOp::PBO_None);
		playerArmorValue += 5;

	}

	if (type == "Shoulder") {
		shoulderLHolder->SetRelativeRotation(rotation[0]);
		shoulderLHolder->SetStaticMesh(itemToEquip[0]);
		shoulderRHolder->SetRelativeRotation(rotation[1]);
		shoulderRHolder->SetStaticMesh(itemToEquip[1]);
		playerArmorValue += 5;

	}

	if (type == "Glove") {
		gloveLHolder->SetRelativeRotation(rotation[0]);
		gloveLHolder->SetStaticMesh(itemToEquip[0]);
		gloveRHolder->SetRelativeRotation(rotation[1]);
		gloveRHolder->SetStaticMesh(itemToEquip[1]);
		playerSkeletalMesh->HideBoneByName("lowerarm_r", EPhysBodyOp::PBO_None);
		playerSkeletalMesh->HideBoneByName("lowerarm_l", EPhysBodyOp::PBO_None);
		playerArmorValue += 5;

	}

	if (type == "Belt") {
		beltHolder->SetRelativeRotation(rotation[0]);
		beltHolder->SetStaticMesh(itemToEquip[0]);
		playerArmorValue += 5;

	}


	if (type == "Pants") {
		pantsHolder->SetRelativeRotation(rotation[0]);
		pantsHolder->SetStaticMesh(itemToEquip[0]);
		playerArmorValue += 5;

	}

	if (type == "ThighArmor") {
		thighArmorLHolder->SetRelativeRotation(rotation[0]);
		thighArmorLHolder->SetStaticMesh(itemToEquip[0]);
		thighArmorRHolder->SetRelativeRotation(rotation[1]);
		thighArmorRHolder->SetStaticMesh(itemToEquip[1]);
		playerArmorValue += 5;

	}

}

//works the same like EquipItem with the difference that the mesh of the slot is changed to nullptr and the stats are removed
void AMyPlayer::DeEquipItem(FName type) {
	if (type == "Sword") {
		playerSwordHolder->SetStaticMesh(nullptr);
		lightAttackDamage = lightAttackDamage / 3.0f;

	}

	if (type == "Shield") {
		playerShieldHolder->SetStaticMesh(nullptr);
		playerArmorValue -= 5;

	}

	if (type == "Armor") {
		playerArmorHolder->SetStaticMesh(nullptr);
		playerArmorValue -= 5;

	}

	if (type == "Boots") {
		bootLHolder->SetStaticMesh(nullptr);
		bootRHolder->SetStaticMesh(nullptr);
		playerSkeletalMesh->UnHideBoneByName("calf_r");
		playerSkeletalMesh->UnHideBoneByName("calf_l");
		playerArmorValue -= 5;

	}

	if (type == "Shoulder") {
		shoulderLHolder->SetStaticMesh(nullptr);
		shoulderRHolder->SetStaticMesh(nullptr);
		playerArmorValue -= 5;

	}

	if (type == "Glove") {
		gloveLHolder->SetStaticMesh(nullptr);
		gloveRHolder->SetStaticMesh(nullptr);
		playerSkeletalMesh->UnHideBoneByName("lowerarm_r");
		playerSkeletalMesh->UnHideBoneByName("lowerarm_l");
		playerArmorValue -= 5;

	}

	if (type == "Belt") {
		beltHolder->SetStaticMesh(nullptr);
		playerArmorValue -= 5;

	}


	if (type == "Pants") {
		pantsHolder->SetStaticMesh(nullptr);
		playerArmorValue -= 5;

	}

	if (type == "ThighArmor") {
		thighArmorLHolder->SetStaticMesh(nullptr);
		thighArmorRHolder->SetStaticMesh(nullptr);
		playerArmorValue -= 5;

	}

}

//called if player is currently attacking
void AMyPlayer::HandleAttacking(float DeltaTime) {
	//if either of attack animations is currently playing
	if (isLightAttacking1 || isLightAttacking2) {
		//increases the time that tracks the animation playing
		currentAttackDuration += DeltaTime;
		//damages the enemies
		for (int i = 0; i < overlappingEnemies.Num(); i++) {
			//makes sure the enemy takes damage only once during one player attack
			if (!overlappingEnemies[i]->alreadyTookDamageFromAttack) {
				overlappingEnemies[i]->MyTakeDamage(lightAttackDamage);
				overlappingEnemies[i]->alreadyTookDamageFromAttack = true;
			}
		}
	}
	//if player stopped attacking
	else {
		//the attack timer gets reseted
		currentAttackDuration = 0.0f;
		//and all the enemies can take damage again, they can take damage again everytime the player animation changes
		for (int i = 0; i < overlappingEnemies.Num(); i++) {
			overlappingEnemies[i]->alreadyTookDamageFromAttack = false;
		}
	}

	//makes sure the stamina gets removed properly and the enemies can be attacked again once the animation finishes
	if (isLightAttacking1) {
		if (currentAttackDuration >= lightAttack1AnimLenght) {
			isLightAttacking1 = false;
			currentAttackDuration = 0.0f;
			if (isLightAttacking2 && shouldRemoveStaminaWhenTheNextAttackStarts) {
				shouldRemoveStaminaAfterLightAttack = true;
				shouldRemoveStaminaWhenTheNextAttackStarts = false;
				for (int i = 0; i < overlappingEnemies.Num(); i++) {
					overlappingEnemies[i]->alreadyTookDamageFromAttack = false;
				}
			}
		}
	}

	//makes sure the stamina gets removed properly and the enemies can be attacked again once the animation finishes
	if (isLightAttacking2) {
		if (currentAttackDuration >= lightAttack2AnimLenght) {
			isLightAttacking2 = false;
			currentAttackDuration = 0.0f;
			if (isLightAttacking1 && shouldRemoveStaminaWhenTheNextAttackStarts) {
				shouldRemoveStaminaAfterLightAttack = true;
				shouldRemoveStaminaWhenTheNextAttackStarts = false;
				for (int i = 0; i < overlappingEnemies.Num(); i++) {
					overlappingEnemies[i]->alreadyTookDamageFromAttack = false;
				}
			}
		}
	}
}

void AMyPlayer::HandleDeath() {
	//if player health falls to 0 he dies
	if (currentPlayerHealth <= 0) {
		//the timer is only set once
		//after that it just waits to restart the level in RestartLevelAfterPlayerDie function
		if (GetWorldTimerManager().IsTimerActive(restartLevelTimerHandle) == false) {
			isDying = true;
			GetWorld()->GetTimerManager().SetTimer(restartLevelTimerHandle, this, &AMyPlayer::RestartLevelAfterPlayerDie, restartLevelDuration, false);
		}
	}
}

void AMyPlayer::HandleCamera() {
	//if player is locked onto an enemy
	if (lockedOnEnemy) {
		//gets the rotation to look at the enemy
		FRotator newRot = UKismetMathLibrary::FindLookAtRotation(cameraAttach->GetComponentLocation(), currentlyLockedOnEnemy->GetActorLocation());
		//sets the proper rotation to look at the enemy for the camera
		cameraAttach->SetWorldRotation(FRotator(newRot.Pitch, newRot.Yaw - 5, newRot.Roll));
		//sets the proper rotation to look at the enemy for the player
		playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 90, 0), 0.03f, 10.0f));
	}
	//makes sure the camera doesn't go too far up or too far down
	cameraAttach->SetWorldRotation(FRotator(FMath::Clamp(cameraAttach->GetComponentRotation().Pitch, -55.0f, 55.0f), cameraAttach->GetComponentRotation().Yaw, 0));
}

void AMyPlayer::HandleDialog() {
	//the last NPC has a special dialog option
	for (int i = 0; i < dialogAI.Num(); i++) {
		if (dialogAI[i]->last == false) {
			// if the NPC is not the last
			//the normal dialog option is displayed
			if (dialogAI[i]->dialogIndex + 1 < dialogAI[i]->dialogNotAllRescued.Num()) {
				//goes through all the dialog texts of the NPC
				dialogAI[i]->dialogIndex++;
				dialogAI[i]->textComp->SetText(FText::FromString(dialogAI[i]->dialogNotAllRescued[dialogAI[i]->dialogIndex]));
				dialogAI[i]->textComp->SetTextRenderColor(FColor::Black);

				//after the NPC says everything it sets a timer
				//when the timer runs out the NPC gets deleted
				if (dialogAI[i]->dialogIndex + 1 == dialogAI[i]->dialogNotAllRescued.Num()) {
					if (GetWorldTimerManager().IsTimerActive(dialogAI[i]->destroyTimerHandle) == false) {
						GetWorldTimerManager().SetTimer(dialogAI[i]->destroyTimerHandle, dialogAI[i], &AMyQuestGiveAI::Delete, 10.0f, false);
					}
				}
			}
		}
		//if the NPC is the last one the special dialog is displayed
		//all the other stuff is the same
		else {
			if (dialogAI[i]->dialogIndex + 1 < dialogAI[i]->dialogAllRescued.Num()) {
				dialogAI[i]->dialogIndex++;
				dialogAI[i]->textComp->SetText(FText::FromString(dialogAI[i]->dialogAllRescued[dialogAI[i]->dialogIndex]));
				dialogAI[i]->textComp->SetTextRenderColor(FColor::Black);

				if (dialogAI[i]->dialogIndex + 1 == dialogAI[i]->dialogAllRescued.Num()) {
					if (GetWorldTimerManager().IsTimerActive(dialogAI[i]->destroyTimerHandle) == false) {
						dialogAI[i]->manager->giveReward = true;
						GetWorldTimerManager().SetTimer(dialogAI[i]->destroyTimerHandle, dialogAI[i], &AMyQuestGiveAI::Delete, 10.0f, false);
					}
				}
			}
		}
	}
}

//all the NPC player can talk to are tracked in an array
//element get added to the array if he overlaps with playerDialog mesh
void AMyPlayer::OnOverlapBeginDialog(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (Cast<AMyQuestGiveAI>(OtherActor) != nullptr) {
		if (dialogAI.Contains(Cast<AMyQuestGiveAI>(OtherActor)) == false) {
			dialogAI.AddUnique(Cast<AMyQuestGiveAI>(OtherActor));
		}
	}

}

//all the NPC player can talk to are tracked in an array
//element get removed from the array if he stops overlapping with playerDialog mesh
void AMyPlayer::OnOverlapEndDialog(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AMyQuestGiveAI>(OtherActor) != nullptr) {
		if (dialogAI.Contains(Cast<AMyQuestGiveAI>(OtherActor))) {
			dialogAI.Remove(Cast<AMyQuestGiveAI>(OtherActor));
		}
	}
	if (dialogAI.Num() == 0) {
		if (hud == nullptr) {
			hud = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AMyHUD>();
		}

		else {
			if (hud && hud->dialogWidget != nullptr) {
				if (hud->dialogWidget->IsInViewport()) {
					hud->ManageDialogWidget(true);
				}
			}
		}
	}

}
//shows and hides the ESC menu based on whether it is shown or hidden
void AMyPlayer::ESCMenuManage() {
	if (hud == nullptr) {
		hud = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AMyHUD>();
	}

	else {
		if (hud) {
			hud->ManageESCMenu();
		}
	}
}