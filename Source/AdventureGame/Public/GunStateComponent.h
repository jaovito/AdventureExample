// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunStateComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ADVENTUREGAME_API UGunStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGunStateComponent();

	UPROPERTY(BlueprintReadWrite)
	bool bIsAiming = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsReloading = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsShooting = false;

	UFUNCTION(BlueprintPure)
	bool CanShoot() const;
};
