// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CombatAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THETHINGSWEDOFORLOVE_API UCombatAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void NativeInitializeAnimation() override;

	void SetSprinting(bool bIsSprinting);

	void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetMovement(const FVector2D& rawInput);

	void SetLockOnTarget(bool bLockedOn);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
	FVector2D Velocity2D;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
	bool bSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
	bool bIsAlert;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Movement")
	bool bIsLockedOnTarget;

	UPROPERTY()
	FVector PreviousLocation;
	
};
