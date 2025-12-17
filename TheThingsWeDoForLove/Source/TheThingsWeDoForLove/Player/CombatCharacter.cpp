#include "CombatCharacter.h"
#include "CombatAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/AudioComponent.h"

// Sets default values
ACombatCharacter::ACombatCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	AttackIndex = 0;

	CombatAnimInstance = Cast<UCombatAnimInstance>(GetMesh()->GetAnimInstance());
	CameraBoom = FindComponentByClass<USpringArmComponent>();
	FollowCamera = FindComponentByClass<UCameraComponent>();
	WalkingSoundComponent = FindComponentByClass<UAudioComponent>();

	AttackCooldownTimer = 0.0f;
	DodgeTimer = 0.0f;
	LastLocation = GetActorLocation();

	GetCharacterMovement()->MaxWalkSpeed = RegularWalkSpeed;

}

// Called every frame
void ACombatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AttackCooldownTimer > 0)
		AttackCooldownTimer -= DeltaTime;

	if (DodgeTimer > 0)
		DodgeTimer -= DeltaTime;



	SmoothRotateToTarget(DeltaTime);

}

void ACombatCharacter::Move(const FVector& Direction, const FVector2D& rawInput)
{
	if (CombatAnimInstance)
	{
		CombatAnimInstance->SetMovement(rawInput);
	}

	if (AttackCooldownTimer > 0 || DodgeTimer > 0)
		return;

	AddMovementInput(Direction);

	if (!bIsLockedOn)
		TargetRotation = Direction.Rotation();

}

void ACombatCharacter::Look(const FVector2D& AxisValues)
{
	AddControllerYawInput(AxisValues.X);
	AddControllerPitchInput(AxisValues.Y);
}

void ACombatCharacter::Sprint(bool bIsSprinting)
{
	if (CombatAnimInstance)
	{
		CombatAnimInstance->SetSprinting(bIsSprinting);
	}

	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : RegularWalkSpeed;
}

void ACombatCharacter::Dodge(const FVector& Direction)
{
	if (AttackCooldownTimer > 0 || DodgeTimer > 0)
	{
		return;
	}

	DodgeTimer = FMath::Max(0, DodgeMontage->GetPlayLength() - DodgeCooldownOffset);

	TargetRotation = Direction.Rotation();
	PlayAnimMontage(DodgeMontage);

	if (DodgeSound)
	{
		UGameplayStatics::PlaySound2D(this, DodgeSound);
	}
}

void ACombatCharacter::Attack(const FVector& Direction)
{
	if (AttackCooldownTimer > 0 || DodgeTimer > 0)
	{
		return;
	}

	AttackCooldownTimer = MeleeAttackCooldown;
	PerformMeleeAttack();
	PlayMeleeAttackMontage();

	TargetRotation = Direction.Rotation();
}

void ACombatCharacter::ToggleLock()
{
	if (bIsLockedOn)
	{
		bIsLockedOn = false;
		CameraBoom->bUsePawnControlRotation = true;

		if (CombatAnimInstance)
		{
			CombatAnimInstance->SetLockOnTarget(false);
		}
		return;
	}

	FHitResult HitResult;
	if (UKismetSystemLibrary::SphereTraceSingle(
		this,
		FollowCamera->GetComponentLocation(),
		FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * LockOnRange,
		LockOnRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		TArray<AActor*>({ GetOwner() }),
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	))
	{
		if (HitResult.GetActor() && HitResult.GetActor()->Implements<UDamageableInterface>())
		{
			LockedTarget = HitResult.GetActor();
			bIsLockedOn = true;
			CameraBoom->bUsePawnControlRotation = false;

			if (CombatAnimInstance)
			{
				CombatAnimInstance->SetLockOnTarget(true);
			}
		}
	}
}

bool ACombatCharacter::ReceiveDamage_Implementation(float DamageAmount)
{
	Health -= DamageAmount;
	if (Health <= 0)
	{
		// Handle death (not implemented)
	}
	return true;
}

void ACombatCharacter::SmoothRotateToTarget(float DeltaTime)
{
	if (bIsLockedOn)
	{
		FVector targetLocation = LockedTarget->GetActorLocation();
		FVector directionToTarget = targetLocation - GetActorLocation();
		FRotator lookAtRotation = directionToTarget.Rotation();

		if (CameraBoom)
		{
			FVector cameraDirectionToTarget = targetLocation - CameraBoom->GetComponentLocation();
			FRotator cameraLookAtRotation = cameraDirectionToTarget.Rotation();
			CameraBoom->SetWorldRotation(cameraLookAtRotation);
		}

		if (FollowCamera)
			FollowCamera->SetRelativeRotation(FRotator::ZeroRotator);

		if (DodgeTimer < 0)
			TargetRotation = lookAtRotation;
	}
	else 
	{
		if(FollowCamera)
			FollowCamera->SetRelativeRotation(FRotator::ZeroRotator);
	}

	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, SmoothRotationSpeed);
	NewRotation.Pitch = 0.0f;

	SetActorRotation(NewRotation);
}

void ACombatCharacter::PerformMeleeAttack()
{
	if (SwordSwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwordSwingSound);
	}


	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		GetActorLocation() + GetActorForwardVector() * MeleeAttackRange,
		MeleeAttackRange,
		TArray<TEnumAsByte<EObjectTypeQuery>>(),
		ACharacter::StaticClass(),
		TArray<AActor*>({ this }),
		OutActors
	);

	if (OutActors.Num() == 1 && OutActors[0] == this)
	{
		return;
	}

	for (auto actor : OutActors)
	{
		if (actor->Implements<UDamageableInterface>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *actor->GetName());
			bool succesFullHit = IDamageableInterface::Execute_ReceiveDamage(actor, MeleeAttackDamage);
			if (succesFullHit && HitCameraShake)
			{
				TObjectPtr<APlayerController> PC{ Cast<APlayerController>(GetController()) };
				if (PC)
				{
					PC->ClientStartCameraShake(HitCameraShake);
				}

				if (HitSound)
				{
					UGameplayStatics::PlaySound2D(this, HitSound);
				}
			}
		}
	}
}

void ACombatCharacter::PlayMeleeAttackMontage()
{
	auto animMontage = MeleeAttackMontages[AttackIndex];
	if (animMontage)
	{
		PlayAnimMontage(animMontage);
		AttackIndex++;
		if (AttackIndex >= MeleeAttackMontages.Num())
			AttackIndex = 0;
	}

	// Move character forward slightly during attack
	FVector ForwardMovement = GetActorForwardVector() * MeleeTravelDistance;
	AddActorWorldOffset(ForwardMovement, true);
}

