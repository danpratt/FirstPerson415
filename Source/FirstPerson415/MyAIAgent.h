// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyAIAgent.generated.h"

class UPawnSensingComponent;
class AFirstPerson415Projectile;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class FIRSTPERSON415_API AMyAIAgent : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyAIAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Function to fire at player
	UFUNCTION()
	void Fire(FRotator AimRotation);

	// Gun Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* GunMesh;

	// Projectile
	UPROPERTY(EditDefaultsOnly, category = "Projectile")
	TSubclassOf<AFirstPerson415Projectile> ProjectileClass;

	// The color the AI will shoot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	FLinearColor DecalColor;

	// AI Perception component for sight
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AIPerceptionComp;

	// Sight config for AI Perception
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* SightConfig;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Add this declaration to your AMyAIAgent class in MyAIAgent.h
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
