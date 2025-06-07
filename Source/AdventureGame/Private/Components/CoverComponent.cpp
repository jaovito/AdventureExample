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
    const FVector Start = OwnerCharacter->GetActorLocation();
    const FVector Forward = OwnerCharacter->GetActorForwardVector();
    const FVector End = Start + Forward * 150.0f;

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
    const FVector Forward = -CoverNormal; // olhando para a parede
    const FRotator TargetRot = Forward.Rotation();

    OwnerCharacter->SetActorLocation(CoverLocation - Forward * 50.0f); // ajusta distância
    OwnerCharacter->SetActorRotation(TargetRot);
}

bool UCoverComponent::IsInCover()
{
    return CurrentState == ECoverState::InCover;
}

void UCoverComponent::MoveAlongCover(float Value)
{
    if (CurrentState != ECoverState::InCover || Value == 0.0f)
        return;

    // Calcula a direção paralela à parede usando o produto vetorial
    FVector Up = FVector::UpVector;
    FVector WallRight = FVector::CrossProduct(CoverNormal, Up);
    WallRight.Normalize();

    // Aplica o valor de input na direção correta
    FVector MoveDirection = WallRight * Value;
    
    // Usa a velocidade de cobertura
    float Speed = CoverMovementSpeed * GetWorld()->GetDeltaSeconds();
    
    // Calcula a nova posição
    FVector CurrentLocation = OwnerCharacter->GetActorLocation();
    FVector NewLocation = CurrentLocation + (MoveDirection * Speed);
    
    // Verifica se ainda há parede na frente no próxima posição do player
    // Adiciona um offset na direção do movimento para detectar a borda antes
    FVector TraceStart = NewLocation + (MoveDirection.GetSafeNormal() * 50.0f); // 50cm à frente na direção do movimento
    FVector TraceEnd = TraceStart + (-CoverNormal * 150.0f); // Usa a direção perpendicular à parede
    
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter);
    
    bool bHasWallAhead = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
    
    // Debug visual
    FColor TraceColor = bHasWallAhead ? FColor::Green : FColor::Red;
    DrawDebugLine(GetWorld(), TraceStart, TraceEnd, TraceColor, false, 0.1f, 0, 2.0f);
    
    // Se não há parede à frente, não se move
    if (!bHasWallAhead)
    {
        return;
    }
    
    // Move o personagem
    OwnerCharacter->AddMovementInput(WallRight, Value * Speed);
    
    // // Atualiza a rotação para sempre olhar para a parede
    // FRotator TargetRot = (-CoverNormal).Rotation();
    // OwnerCharacter->SetActorRotation(TargetRot);
}