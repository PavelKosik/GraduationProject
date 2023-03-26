// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayer.h"
#include "MyEnemyCorpseCharacter.h"

AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	playerSkeletalMesh = GetMesh();
	capsule = GetCapsuleComponent();
	movementComponent = GetCharacterMovement();

	RootComponent = capsule;
	playerAttackHitbox = CreateDefaultSubobject<UStaticMeshComponent>("PlayerAttackHitbox");

	cameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	cameraAttach = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	playerSkeletalMesh->SetupAttachment(RootComponent);
	playerAttackHitbox->SetupAttachment(playerSkeletalMesh);
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


		if (isRunning && currentStamina <= 0.0f) {
			isRunning = false;
		}

		AMyPlayer::HandleDeath();
	}

	if (horizontalForRoll == 0 && verticalForRoll == 0 && currentStaminaDrain == sprintStaminaDrain) {
		currentStaminaDrain = 0.0f;
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
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AMyPlayer::BlockStart);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AMyPlayer::BlockEnd);
}

void AMyPlayer::MoveHorizontal(float horizontal) {
	if (!isStaggered) {

		if (!isRolling) {
			if (!isLightAttacking1 && !isLightAttacking2) {
				horizontalForRoll = horizontal;

				FRotator rightRotation = cameraAttach->GetComponentRotation();
				FRotator yawRotation(0.0f, rightRotation.Yaw, 0.0f);

				FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

				if (isRunning) {
					if (horizontal != 0) {
						currentStaminaDrain = sprintStaminaDrain;
						movementComponent->MaxWalkSpeed = runningSpeed;
						AddMovementInput(rightDirection, horizontal);
					}
				}

				else {
					movementComponent->MaxWalkSpeed = playerMovespeed;
					AddMovementInput(rightDirection, horizontal);
				}

				if (horizontal != 0) {
					isMoving = true;
					AMyPlayer::RotateToMovementHorizontal(horizontal);

				}
			}
		}
	}
}

void AMyPlayer::MoveVertical(float vertical) {
	if (!isStaggered) {

		if (!isRolling) {
			if (!isLightAttacking1 && !isLightAttacking2) {

				verticalForRoll = vertical;

				FRotator forwardRotation = cameraAttach->GetComponentRotation();
				FRotator yawRotation(0.0f, forwardRotation.Yaw, 0.0f);

				FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);


				if (isRunning) {
					if (vertical != 0) {
						currentStaminaDrain = sprintStaminaDrain;
						movementComponent->MaxWalkSpeed = runningSpeed;
						AddMovementInput(forwardDirection, vertical);
					}
				}


				else {
					movementComponent->MaxWalkSpeed = playerMovespeed;
					AddMovementInput(forwardDirection, vertical);
				}

				if (vertical != 0) {
					isMoving = true;
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
	//cameraAttach->SetWorldRotation(cameraAttach->GetComponentRotation() + FRotator(0, 0, cameraRotationSpeed*horizontal));

	//playerSkeletalMesh->SetWorldRotation(FRotator(0, 0, 0));
	//cameraComponent->SetWorldLocation(FVector(cameraComponent->GetComponentLocation().X * horizontal, cameraComponent->GetComponentLocation().Y, cameraComponent->GetComponentLocation().Z));
}

void AMyPlayer::CameraVertical(float vertical) {
	if (!lockedOnEnemy) {
		cameraAttach->AddLocalRotation(FRotator(cameraRotationSpeed * vertical, 0, 0));
	}
	//playerSkeletalMesh->SetWorldRotation(FRotator(0, 0, 0));

}

void AMyPlayer::RotateToMovementVertical(float vertical) {
	if (!lockedOnEnemy) {
		if (vertical > 0) {
			playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 90, 0), 0.03f, 10.0f));
		}

		if (vertical < 0) {
			playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 270, 0), 0.03f, 10.0f));
		}
	}

}


void AMyPlayer::RotateToMovementHorizontal(float horizontal) {
	if (!lockedOnEnemy) {

		if (horizontal < 0) {
			playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 180, 0), 0.03f, 10.0f));
		}

		if (horizontal > 0)
		{
			playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 360, 0), 0.03f, 10.0f));
		}
	}
}

void AMyPlayer::StartRun() {
	if (currentStamina > 0.0f) {
		isRunning = true;
	}
}

void AMyPlayer::StopRun() {
	isRunning = false;
}

void AMyPlayer::LightAttack() {
	if (!isStaggered) {

		if (currentStamina >= lightAttackStaminaDrain) {
			if (isLightAttacking1 == false) {
				if (isLightAttacking2) {
					shouldRemoveStaminaWhenTheNextAttackStarts = true;
				}
				else {
					shouldRemoveStaminaAfterLightAttack = true;
				}
				isLightAttacking1 = true;

				if (GetWorldTimerManager().IsTimerActive(lightAttackTimerHandle) == false) {
					GetWorld()->GetTimerManager().SetTimer(lightAttackTimerHandle, this, &AMyPlayer::DoLightAttack, lightAttack1AnimLenght, false);

				}

				else {
					GetWorld()->GetTimerManager().ClearTimer(lightAttackTimerHandle);
					GetWorld()->GetTimerManager().SetTimer(lightAttackTimerHandle, this, &AMyPlayer::DoLightAttack, lightAttack1AnimLenght, false);
				}
			}
			else {
				if (isLightAttacking2 == false) {
					if (isLightAttacking1) {
						shouldRemoveStaminaWhenTheNextAttackStarts = true;
					}

					else {
						shouldRemoveStaminaAfterLightAttack = true;
					}

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

	isLightAttacking1 = false;
	isLightAttacking2 = false;

}

void AMyPlayer::HandleStamina(float DeltaTime) {

	if (shouldRemoveStaminaAfterLightAttack) {
		currentStaminaRecoveryTime = 0.0f;
		currentStamina -= lightAttackStaminaDrain;
	}

	currentStamina -= currentStaminaDrain;
	shouldRemoveStaminaAfterLightAttack = false;

	if (!isRunning) {
		currentStaminaDrain = 0.0f;
	}

	if (currentStaminaDrain == 0.0f) {
		currentStaminaRecoveryTime += DeltaTime;
	}

	else {
		currentStaminaRecoveryTime = 0.0f;
	}

	if (currentStaminaRecoveryTime >= timeBeforeStaminaRecovery) {
		currentStamina += staminaRecoverySpeed;
	}

	if (currentStamina < 0.0f) {
		currentStamina = 0.0f;
	}

	if (currentStamina > maxStamina) {
		currentStamina = maxStamina;
	}

}

void AMyPlayer::StartRoll() {
	if (!isLightAttacking1 && !isLightAttacking2) {

		if (!isRolling) {
			if (currentStamina >= rollStaminaDrain) {
				isRolling = true;
				currentRollTime = 0.0f;
				currentStamina -= rollStaminaDrain;
				currentStaminaRecoveryTime = 0.0f;
				HandleRolling();
			}
		}
	}
}

void AMyPlayer::HandleRolling() {

	if (isRolling) {

		FRotator rotation = playerSkeletalMesh->GetComponentRotation();
		FRotator yawRotation(0, rotation.Yaw, 0);
		FVector direction = FVector(0, 0, 0);
		if (lockedOnEnemy) {

			if (horizontalForRoll != 0) {
				direction += FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X) * -horizontalForRoll;
			}

			if (verticalForRoll != 0) {
				direction += FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y) * verticalForRoll;
			}
		}

		if (direction.IsZero()) {
			direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		}

		movementComponent->MaxWalkSpeed = rollSpeed;
		AddMovementInput(direction, 1);

		currentRollTime += deltaTimeForRolling;

	}
	if (currentRollTime >= rollAnimDuration) {
		isRolling = false;
	}
}

void AMyPlayer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (Cast<AMyEnemyCorpseCharacter>(OtherActor) != nullptr) {
		overlappingEnemies.AddUnique(Cast<AMyEnemyCorpseCharacter>(OtherActor));
	}

}

void AMyPlayer::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AMyEnemyCorpseCharacter>(OtherActor) != nullptr) {
		overlappingEnemies.Remove(Cast<AMyEnemyCorpseCharacter>(OtherActor));
	}

}

void AMyPlayer::LockOnEnemy() {
	bool foundEnemyToLockOn = false;
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> ignoreActors;

	ignoreActors.Init(this, 1);
	AActor* enemyToLockOn = nullptr;

	TArray<AActor*> outActors;
	UKismetSystemLibrary::SphereOverlapActors(GetOwner(), playerSkeletalMesh->GetComponentLocation(), maxTargetLockDistance, traceObjectTypes, AMyEnemyCorpseCharacter::StaticClass(), ignoreActors, outActors);

	if (lockedOnEnemy) {
		class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
		enemy->HideHealthBar();
		lockedOnEnemy = false;
		currentlyLockedOnEnemy = nullptr;
		foundEnemyToLockOn = false;
		return;
	}

	else {
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
							//	if (dot > closestDotToCenter) {
							closestDotToCenter = dot;
							foundEnemyToLockOn = true;
							enemyToLockOn = outActors[i];

							//}
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
	}



	if (enemyToLockOn != nullptr) {
		if (currentlyLockedOnEnemy != enemyToLockOn) {
			currentlyLockedOnEnemy = enemyToLockOn;
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
			enemy->ShowHealthBar();
		}

		else {
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
			enemy->HideHealthBar();

			foundEnemyToLockOn = false;
			lockedOnEnemy = false;
			currentlyLockedOnEnemy = nullptr;

		}
	}



	if (foundEnemyToLockOn) {
		lockedOnEnemy = true;
	}

	else {
		//make camera look player forward
		cameraAttach->SetWorldRotation(FRotator(FMath::Clamp(cameraAttach->GetComponentRotation().Pitch, -55.0f, 55.0f), playerSkeletalMesh->GetComponentRotation().Yaw + 90.0f, 0));
	}
}

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
			enemy->ShowHealthBar();
		}

		else {
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
			enemy->HideHealthBar();

			foundEnemyToLockOn = false;
			lockedOnEnemy = false;
			currentlyLockedOnEnemy = nullptr;

		}
	}

	else {
		if (currentlyLockedOnEnemy != nullptr) {
			class AMyEnemyCorpseCharacter* enemy = Cast<AMyEnemyCorpseCharacter>(currentlyLockedOnEnemy);
			enemy->HideHealthBar();
		}
		foundEnemyToLockOn = false;
		lockedOnEnemy = false;
		currentlyLockedOnEnemy = nullptr;
	}

}

void AMyPlayer::Stagger() {
	isStaggered = true;
	GetWorld()->GetTimerManager().SetTimer(staggerTimerHandle, this, &AMyPlayer::StopStagger, staggerDuration, false);

}


void AMyPlayer::StopStagger() {
	isStaggered = false;
}

void AMyPlayer::RestartLevelAfterPlayerDie() {
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AMyPlayer::BlockStart() {
	blockTime = 0.0f;
	isBlocking = true;
}

void AMyPlayer::BlockEnd() {
	isBlocking = false;
}

void AMyPlayer::EquipItem(TArray<UStaticMesh*> itemToEquip, TArray<FRotator> rotation, FName type) {

	if (type == "Sword") {
		playerSwordHolder->SetRelativeRotation(rotation[0]);
		playerSwordHolder->SetStaticMesh(itemToEquip[0]);
	}

	if (type == "Shield") {
		playerShieldHolder->SetRelativeRotation(rotation[0]);
		playerShieldHolder->SetStaticMesh(itemToEquip[0]);
	}

	if (type == "Armor") {
		playerArmorHolder->SetRelativeRotation(rotation[0]);
		playerArmorHolder->SetStaticMesh(itemToEquip[0]);
	}

	if (type == "Boots") {
		bootLHolder->SetRelativeRotation(rotation[0]);
		bootLHolder->SetStaticMesh(itemToEquip[0]);
		bootRHolder->SetRelativeRotation(rotation[1]);
		bootRHolder->SetStaticMesh(itemToEquip[1]);
		playerSkeletalMesh->HideBoneByName("calf_r", EPhysBodyOp::PBO_None);
		playerSkeletalMesh->HideBoneByName("calf_l", EPhysBodyOp::PBO_None);
	}

	if (type == "Shoulder") {
		shoulderLHolder->SetRelativeRotation(rotation[0]);
		shoulderLHolder->SetStaticMesh(itemToEquip[0]);
		shoulderRHolder->SetRelativeRotation(rotation[1]);
		shoulderRHolder->SetStaticMesh(itemToEquip[1]);
	}

	if (type == "Glove") {
		gloveLHolder->SetRelativeRotation(rotation[0]);
		gloveLHolder->SetStaticMesh(itemToEquip[0]);
		gloveRHolder->SetRelativeRotation(rotation[1]);
		gloveRHolder->SetStaticMesh(itemToEquip[1]);
		playerSkeletalMesh->HideBoneByName("lowerarm_r", EPhysBodyOp::PBO_None);
		playerSkeletalMesh->HideBoneByName("lowerarm_l", EPhysBodyOp::PBO_None);
	}

	if (type == "Belt") {
		beltHolder->SetRelativeRotation(rotation[0]);
		beltHolder->SetStaticMesh(itemToEquip[0]);
	}


	if (type == "Pants") {
		pantsHolder->SetRelativeRotation(rotation[0]);
		pantsHolder->SetStaticMesh(itemToEquip[0]);
	}

	if (type == "ThighArmor") {
		thighArmorLHolder->SetRelativeRotation(rotation[0]);
		thighArmorLHolder->SetStaticMesh(itemToEquip[0]);
		thighArmorRHolder->SetRelativeRotation(rotation[1]);
		thighArmorRHolder->SetStaticMesh(itemToEquip[1]);
	}

}

void AMyPlayer::DeEquipItem(FName type) {
	if (type == "Sword") {
		playerSwordHolder->SetStaticMesh(nullptr);
	}

	if (type == "Shield") {
		playerShieldHolder->SetStaticMesh(nullptr);
	}

	if (type == "Armor") {
		playerArmorHolder->SetStaticMesh(nullptr);
	}

	if (type == "Boots") {
		bootLHolder->SetStaticMesh(nullptr);
		bootRHolder->SetStaticMesh(nullptr);
		playerSkeletalMesh->UnHideBoneByName("calf_r");
		playerSkeletalMesh->UnHideBoneByName("calf_l");
	}

	if (type == "Shoulder") {
		shoulderLHolder->SetStaticMesh(nullptr);
		shoulderRHolder->SetStaticMesh(nullptr);
	}

	if (type == "Glove") {
		gloveLHolder->SetStaticMesh(nullptr);
		gloveRHolder->SetStaticMesh(nullptr);
		playerSkeletalMesh->UnHideBoneByName("lowerarm_r");
		playerSkeletalMesh->UnHideBoneByName("lowerarm_l");
	}

	if (type == "Belt") {
		beltHolder->SetStaticMesh(nullptr);
	}


	if (type == "Pants") {
		pantsHolder->SetStaticMesh(nullptr);
	}

	if (type == "ThighArmor") {
		thighArmorLHolder->SetStaticMesh(nullptr);
		thighArmorRHolder->SetStaticMesh(nullptr);
	}

}

void AMyPlayer::HandleAttacking(float DeltaTime) {
	if (isLightAttacking1 || isLightAttacking2) {
		currentAttackDuration += DeltaTime;

		for (int i = 0; i < overlappingEnemies.Num(); i++) {
			if (!overlappingEnemies[i]->alreadyTookDamageFromAttack) {
				overlappingEnemies[i]->MyTakeDamage(lightAttackDamage);
				overlappingEnemies[i]->alreadyTookDamageFromAttack = true;
			}
		}
	}

	else {
		currentAttackDuration = 0.0f;
		for (int i = 0; i < overlappingEnemies.Num(); i++) {
			overlappingEnemies[i]->alreadyTookDamageFromAttack = false;
		}
	}

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
	if (currentPlayerHealth <= 0) {
		if (GetWorldTimerManager().IsTimerActive(restartLevelTimerHandle) == false) {
			isDying = true;
			GetWorld()->GetTimerManager().SetTimer(restartLevelTimerHandle, this, &AMyPlayer::RestartLevelAfterPlayerDie, restartLevelDuration, false);
		}
	}
}

void AMyPlayer::HandleCamera() {
	if (lockedOnEnemy) {
		FRotator newRot = UKismetMathLibrary::FindLookAtRotation(cameraAttach->GetComponentLocation(), currentlyLockedOnEnemy->GetActorLocation());
		cameraAttach->SetWorldRotation(FRotator(newRot.Pitch, newRot.Yaw - 5, newRot.Roll));
		playerSkeletalMesh->SetWorldRotation(FMath::RInterpTo(playerSkeletalMesh->GetComponentRotation(), FRotator(0, cameraAttach->GetComponentRotation().Yaw - 90, 0), 0.03f, 10.0f));

	}
	cameraAttach->SetWorldRotation(FRotator(FMath::Clamp(cameraAttach->GetComponentRotation().Pitch, -55.0f, 55.0f), cameraAttach->GetComponentRotation().Yaw, 0));
}