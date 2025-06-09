#pragma once

#include "CoreMinimal.h"
#include "Animations/Enums/ECoverState.h"
#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"
#include "CoverComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREGAME_API UCoverComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCoverComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Cover")
	bool TryEnterCover();

	UFUNCTION(BlueprintCallable, Category = "Cover")	
	void ExitCover();

	UFUNCTION(BlueprintCallable, Category = "Cover")
	void MoveAlongCover(float Value);

	UFUNCTION(BlueprintPure, Category = "Cover")
	bool IsInCover();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Cover")
	float CoverCheckDistance = 150.0f;

private:
	ECoverState CurrentState;

	FVector CoverLocation;
	FVector CoverNormal;

	ACharacter* OwnerCharacter;

	// Variáveis para movimento fluído
	bool bMovingToCover;
	FVector TargetLocation;
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, Category = "Cover")
	float CoverMovementSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Cover")
	float CoverTransitionSpeed = 200.0f;

	void AlignToCover();
	bool FindCover(FVector& OutLocation, FVector& OutNormal);
};
