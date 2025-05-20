// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CoverComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UCoverComponent::UCoverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentState = ECoverState::None;
}


void UCoverComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ACharacter>(GetOwner());
}


void UCoverComponent::TryEnterCover() 
{
    if (CurrentState != ECoverState::None)
        return;

	FVector Location;
    FVector Normal;

	if (FindCover(Location, Normal))
    {
        CoverLocation = Location;
        CoverNormal = Normal;
        AlignToCover();
        CurrentState = ECoverState::InCover;

	    // get character movement

		UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();

		OwnerMovement->SetMovementMode(MOVE_None);
		// OwnerMovement->PlayCoverMontage(); // opcional
	}
}

void UCoverComponent::ExitCover()
{
    if (CurrentState == ECoverState::InCover)
    {
        CurrentState = ECoverState::None;
		UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
        OwnerMovement->SetMovementMode(EMovementMode::MOVE_Walking);
        // OwnerCharacter->PlayCoverExitMontage(); // opcional
    }
}

bool UCoverComponent::FindCover(FVector& OutLocation, FVector& OutNormal)
{
    FVector Start = OwnerCharacter->GetActorLocation();
    FVector Forward = OwnerCharacter->GetActorForwardVector();
    FVector End = Start + Forward * 150.0f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    if (bHit && Hit.bBlockingHit)
    {
        OutLocation = Hit.ImpactPoint;
        OutNormal = Hit.ImpactNormal;
        return true;
    }

    return false;
}

void UCoverComponent::AlignToCover()
{
    FVector Forward = -CoverNormal; // olhando para a parede
    FRotator TargetRot = Forward.Rotation();

    OwnerCharacter->SetActorLocation(CoverLocation - Forward * 50.0f); // ajusta distância
    OwnerCharacter->SetActorRotation(TargetRot);
}

bool UCoverComponent::IsInCover()
{
    return CurrentState == ECoverState::InCover;
}
