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

	UFUNCTION(BlueprintCallable, Category = "Cover")
	void TryEnterCover();

	UFUNCTION(BlueprintCallable, Category = "Cover")	
	void ExitCover();

	void MoveAlongCover(float Value);

	UFUNCTION(BlueprintPure, Category = "Cover")
	bool IsInCover();

protected:
	virtual void BeginPlay() override;

private:
	ECoverState CurrentState;

	FVector CoverLocation;
	FVector CoverNormal;

	ACharacter* OwnerCharacter;

	void AlignToCover();
	bool FindCover(FVector& OutLocation, FVector& OutNormal);
};
