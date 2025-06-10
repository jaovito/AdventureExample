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
        // Calculate the direction and distance to the target cover location
        FVector CurrentLocation = OwnerCharacter->GetActorLocation();
        FVector Direction = (TargetLocation - CurrentLocation);
        float Distance = Direction.Size();
        
        // Check if the character is close enough to the cover
        if (Distance < 10.0f)
        {
            bMovingToCover = false;
            CurrentState = ECoverState::InCover;
            OwnerCharacter->SetActorLocation(TargetLocation);
            OwnerCharacter->SetActorRotation(TargetRotation);
        }
        else
        {
            // Move the character towards the cover
            Direction.Normalize();
            OwnerCharacter->AddMovementInput(Direction, 1.0f);
            
            // Smoothly rotate the character towards the cover
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
        // OwnerCharacter->PlayCoverExitMontage(); // optional
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

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    const FColor DebugColor = bHit && Hit.bBlockingHit ? FColor::Green : FColor::Red;

    DrawDebugLine(GetWorld(), Start, End, DebugColor, false, 1.0f);
    
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
    const FVector Forward = -CoverNormal;
    const FRotator TargetRot = Forward.Rotation();

    // Set the character's rotation to face the cover
    TargetLocation = CoverLocation - Forward * 50.0f;
    TargetRotation = TargetRot;
    
    bMovingToCover = true;
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

    // Calculate the right direction along the wall using the cover normal
    FVector Up = FVector::UpVector;
    FVector WallRight = FVector::CrossProduct(CoverNormal, Up);
    WallRight.Normalize();

    // Apply the movement direction based on the input value
    FVector MoveDirection = WallRight * Value;
    
    // Use the CoverMovementSpeed to determine the speed of movement
    float Speed = CoverMovementSpeed * GetWorld()->GetDeltaSeconds();
    
    // Get the current location of the character and calculate the new location
    FVector CurrentLocation = OwnerCharacter->GetActorLocation();
    FVector NewLocation = CurrentLocation + (MoveDirection * Speed);
    
    // Verify if there is a wall ahead
    // Add an offset to the start of the trace to avoid hitting the character itself
    FVector TraceStart = NewLocation + (MoveDirection.GetSafeNormal() * 50.0f); // 50cm à frente na direção do movimento
    FVector TraceEnd = TraceStart + (-CoverNormal * 60.0f); // Usa a direção perpendicular à parede
    
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter);
    
    bool bHasWallAhead = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);
    
    // Debug visual
    FColor TraceColor = bHasWallAhead ? FColor::Green : FColor::Red;
    DrawDebugLine(GetWorld(), TraceStart, TraceEnd, TraceColor, false, 0.1f, 0, 2.0f);
    
    // If there is no wall ahead, do not move
    if (!bHasWallAhead)
    {
        return;
    }
    
    // Move the character along the wall
    OwnerCharacter->AddMovementInput(WallRight, Value * Speed);
    
    // // Update the character's rotation to face the wall
    // FRotator TargetRot = (-CoverNormal).Rotation();
    // OwnerCharacter->SetActorRotation(TargetRot);
}