#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatCharacter.generated.h"

UINTERFACE(Blueprintable)
class THETHINGSWEDOFORLOVE_API UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class THETHINGSWEDOFORLOVE_API IDamageableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	bool ReceiveDamage(float DamageAmount);
};

UCLASS()
class THETHINGSWEDOFORLOVE_API ACombatCharacter : public ACharacter, public IDamageableInterface
{
	GENERATED_BODY()

public:
	ACombatCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Move(const FVector& Direction, const FVector2D& rawInput);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Look(const FVector2D& AxisValues);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Sprint(bool bIsSprinting);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Dodge(const FVector& Direction);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack(const FVector& Direction);

	UFUNCTION(BlueprintCallable, Category = "CombatLocking")
	void ToggleLock();

	virtual bool ReceiveDamage_Implementation(float DamageAmount) override;

private:
	void SmoothRotateToTarget(float DeltaTime);

	void PerformMeleeAttack();

	void PlayMeleeAttackMontage();


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCombatAnimInstance> CombatAnimInstance;

private:
#pragma region Components

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UCameraComponent> FollowCamera;

#pragma endregion

#pragma region Movement

	UPROPERTY(EditAnywhere, Category = "Movement|Settings")
	float RegularWalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Movement|Settings")
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, Category = "Movement|Settings")
	float SmoothRotationSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Movement|Animation")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Movement|Animation")
	float DodgeCooldownOffset = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Movement|Feedback")
	TObjectPtr<class UAudioComponent> WalkingSoundComponent;

	UPROPERTY(EditAnywhere, Category = "Movement|Feedback")
	TObjectPtr<class USoundBase> DodgeSound;

	UPROPERTY()
	FVector LastLocation;

	UPROPERTY()
	float DodgeTimer = -FLT_MAX;

	UPROPERTY()
	FRotator TargetRotation;

#pragma endregion

#pragma region Combat

	UPROPERTY(EditAnywhere, Category = "Combat|Settings")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Combat|Settings")
	float MeleeAttackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Combat|Settings")
	float MeleeAttackDamage = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Combat|Settings")
	float MeleeAttackCooldown = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Combat|Settings")
	float MeleeTravelDistance = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Combat|Animation")
	TArray<TObjectPtr<UAnimMontage>> MeleeAttackMontages;

	UPROPERTY(EditAnywhere, Category="Combat|Feedback")
	TSubclassOf<class UCameraShakeBase> HitCameraShake;

	UPROPERTY(EditAnywhere, Category = "Combat|Feedback")
	TObjectPtr<class USoundBase> SwordSwingSound;

	UPROPERTY(EditAnywhere, Category = "Combat|Feedback")
	TObjectPtr<class USoundBase> HitSound;

	UPROPERTY()
	float Health;

	UPROPERTY()
	int AttackIndex = 0;

	UPROPERTY()
	float AttackCooldownTimer = -FLT_MAX;

#pragma endregion

#pragma region CombatLocking

	UPROPERTY(EditAnywhere, Category = "CombatLocking|Settings")
	float LockOnRange = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "CombatLocking|Settings")
	float LockOnRadius = 500.0f;

	UPROPERTY()
	TObjectPtr<AActor> LockedTarget;

	UPROPERTY()
	bool bIsLockedOn = false;
#pragma endregion
};
