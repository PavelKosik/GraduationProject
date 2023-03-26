// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "Navigation/CrowdAgentInterface.h"
#include "Navigation/CrowdManager.h"
#include "MyPlayer.generated.h"

class MyEnemyCorpseCharacter;
UCLASS()
class MYSCHOOLPROJECT_API AMyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* playerSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent* cameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USpringArmComponent* cameraAttach;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float playerMovespeed{ 300.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float cameraRotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float runningSpeed{ 450.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxStamina{ 100.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float sprintStaminaDrain{ 0.25f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float timeBeforeStaminaRecovery{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float staminaRecoverySpeed{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float lightAttack1AnimLenght{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float lightAttack2AnimLenght{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float lightAttackStaminaDrain{ 10.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rollAnimDuration{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rollStaminaDrain{ 5.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rollSpeed{ 250.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* playerAttackHitbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* playerSwordHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* playerShieldHolder;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* playerArmorHolder;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* bootLHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* bootRHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* shoulderRHolder; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* shoulderLHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* gloveRHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* gloveLHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* thighArmorRHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* thighArmorLHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* beltHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* pantsHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float lightAttackDamage{ 15.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxTargetLockDistance{ 600.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float playerMaxHealth{ 1000.0f };

	float currentPlayerHealth{ playerMaxHealth };

	bool isRolling{ false };

	float currentRollTime{ 0.0f };

	float currentStaminaDrain{ 0.0f };

	float currentStaminaRecoveryTime{ 0.0f };

	float currentStamina{ maxStamina };

	UCapsuleComponent* capsule;

	float rotatedAlready{ 0.0f };

	bool isRunning{ false };

	bool isMoving{ false };

	bool isDying{ false };

	bool isBlocking{ false };

	bool shouldRemoveStaminaAfterLightAttack{ false };

	bool shouldRemoveStaminaWhenTheNextAttackStarts{ false };

	FTimerHandle lightAttackTimerHandle;

	bool isLightAttacking1{ false };
	bool isLightAttacking2{ false };

	float horizontalForRoll, verticalForRoll;

	float currentAttackDuration{ 0.0f };

	float deltaTimeForRolling;

	UCharacterMovementComponent* movementComponent;

	TArray<class AMyEnemyCorpseCharacter*> overlappingEnemies;

	bool lockedOnEnemy{ false };
	AActor* currentlyLockedOnEnemy{ nullptr };

	bool isStaggered{ false };

	FTimerHandle staggerTimerHandle;
	FTimerHandle restartLevelTimerHandle;

	float blockTime{ 0.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float staggerDuration{ 0.25f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float restartLevelDuration{ 2.5f };

	//	bool showInventory{ false };

	void MoveHorizontal(float horizontal);

	void MoveVertical(float vertical);

	void CameraHorizontal(float horizontal);
	void CameraVertical(float vertical);

	void RotateToMovementVertical(float horizontal);
	void RotateToMovementHorizontal(float vertical);

	void StartRun();

	void StopRun();

	void LightAttack();

	void DoLightAttack();

	void HandleStamina(float DeltaTime);

	void StartRoll();
	void HandleRolling();
	void HandleAttacking(float DeltaTime);
	void HandleDeath();
	void HandleCamera();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void LockOnEnemy();

	void LockToDifferentEnemyAfterKill();

	void Stagger();
	void StopStagger();

	void RestartLevelAfterPlayerDie();

	void BlockStart();

	void BlockEnd();

	void EquipItem(TArray<UStaticMesh*> itemToEquip, TArray<FRotator> rotation, FName type);

	void DeEquipItem(FName type);
};
