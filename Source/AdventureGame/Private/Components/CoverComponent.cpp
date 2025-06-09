// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CoverComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UCoverComponent::UCoverComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentState = ECoverState::None;
    bMovingToCover = false;
}


void UCoverComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UCoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (bMovingToCover && OwnerCharacter)
    {
        // Calcula a direção para o alvo
        FVector CurrentLocation = OwnerCharacter->GetActorLocation();
        FVector Direction = (TargetLocation - CurrentLocation);
        float Distance = Direction.Size();
        
        // Se chegou perto o suficiente do alvo
        if (Distance < 10.0f)
        {
            bMovingToCover = false;
            CurrentState = ECoverState::InCover;
            OwnerCharacter->SetActorLocation(TargetLocation);
            OwnerCharacter->SetActorRotation(TargetRotation);
        }
        else
        {
            // Move na direção do alvo usando AddMovementInput
            Direction.Normalize();
            OwnerCharacter->AddMovementInput(Direction, 1.0f);
            
            // Rotação suave para o alvo
            FRotator CurrentRotation = OwnerCharacter->GetActorRotation();
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);
            OwnerCharacter->SetActorRotation(NewRotation);
        }
    }
}

bool UCoverComponent::TryEnterCover() 
{
    if (CurrentState != ECoverState::None || bMovingToCover)
        return false;

	FVector Location;
    FVector Normal;

	if (FindCover(Location, Normal))
    {
        CoverLocation = Location;
        CoverNormal = Normal;
        AlignToCover();
	    return true;
	}

    return false;
}

void UCoverComponent::ExitCover()
{
    if (CurrentState == ECoverState::InCover)
    {
        CurrentState = ECoverState::None;
        bMovingToCover = false;
        
        // Para o Tick quando não está mais movendo
        PrimaryComponentTick.bCanEverTick = false;
        
		UCharacterMovementComponent* OwnerMovement = OwnerCharacter->GetCharacterMovement();
        OwnerMovement->SetMovementMode(EMovementMode::MOVE_Walking);
        // OwnerCharacter->PlayCoverExitMontage(); // opcional
    }
}

bool UCoverComponent::FindCover(FVector& OutLocation, FVector& OutNormal)
{
    const FVector Start = OwnerCharacter->GetActorLocation();
    const FVector Forward = OwnerCharacter->GetActorForwardVector();
    const FVector End = Start + Forward * CoverCheckDistance;

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

    // Define a posição e rotação alvo
    TargetLocation = CoverLocation - Forward * 50.0f; // ajusta distância
    TargetRotation = TargetRot;
    
    // Inicia o movimento fluído
    bMovingToCover = true;
    
    // Garante que o Tick esteja ativo
    PrimaryComponentTick.bCanEverTick = true;
}

bool UCoverComponent::IsInCover()
{
    return CurrentState == ECoverState::InCover;
}

void UCoverComponent::MoveAlongCover(float Value)
{
    if (CurrentState != ECoverState::InCover || Value == 0.0f || bMovingToCover)
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