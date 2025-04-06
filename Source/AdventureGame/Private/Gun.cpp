// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

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