#include "CombatAnimInstance.h"

void UCombatAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	PreviousLocation = FVector::ZeroVector;
	bSprinting = false;
	bIsAlert = false;
}

void UCombatAnimInstance::SetSprinting(bool bIsSprinting)
{
	bSprinting = bIsSprinting;
}

void UCombatAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UCombatAnimInstance::SetMovement(const FVector2D& rawInput)
{

	Velocity2D.X = FMath::Clamp(rawInput.X, -1.0f, 2.0f);
	Velocity2D.Y = FMath::Clamp(rawInput.Y, -1.0f, 2.0f);
	if (bSprinting)
	{
		Velocity2D.X = Velocity2D.X * 2.0f;
		Velocity2D.Y = Velocity2D.Y * 2.0f;
	}
}

void UCombatAnimInstance::SetLockOnTarget(bool bLockedOn)
{
	bIsLockedOnTarget = bLockedOn;
}

