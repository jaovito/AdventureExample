// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Base/Gun.h"

#include "Components/ArrowComponent.h"
#include "Components/GunStateComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SetRootComponent(SkeletalMeshComponent);

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(SkeletalMeshComponent);

	BulletProjectile = CreateDefaultSubobject<UArrowComponent>(TEXT("BulletProjectile"));
	Shells = CreateDefaultSubobject<UArrowComponent>(TEXT("Shells"));
	
	BulletProjectile->SetupAttachment(SkeletalMeshComponent);
	Shells->SetupAttachment(SkeletalMeshComponent);

	// set the color of the bullet projectile
	BulletProjectile->SetArrowColor(FLinearColor::Yellow);
	Shells->SetArrowColor(FLinearColor::Green);

	BulletProjectile->SetArrowSize(0.2);
	Shells->SetArrowSize(0.2);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGun::Pickup(ACharacter* Character)
{
	SkeletalMeshComponent->SetSimulatePhysics(false);
	SkeletalMeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// attach to player
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

	if (Character)
	{
		AttachToComponent(Character->GetMesh(), AttachmentRules, TEXT("hand_rHandgun"));
	}
}

inline void AGun::Shoot(FHitResult HitResult)
{
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UGunStateComponent* GunState = Character->FindComponentByClass<UGunStateComponent>())
		{
			if (GunState->CanShoot())
			{
				GunState->bIsShooting = true;
			}
		}
		else
		{
			GunState->bIsShooting = false;
		}
	}
	
	if (IsValid(HitResult.GetActor()) && IsValid(ProjectileClass))
	{
		// spawn the projectile
		FVector SpawnLocation = BulletProjectile->GetComponentLocation();
		FRotator SpawnRotation = BulletProjectile->GetComponentRotation();
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnInfo);
	}
}
