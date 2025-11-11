// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FirstPerson415Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS(config=Game)
class AFirstPerson415Projectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// Projectile Ball Mesh Component
	UPROPERTY(EditAnywhere, Category = Projectile)
	UStaticMeshComponent* BallMesh;

	// Material to use for splat decal
	UPROPERTY(EditAnywhere, Category = Projectile)
	UMaterial* SplatMaterial;

	// Random color for projectile and decal
	UPROPERTY()
	FLinearColor RandomColor;

	// Material for projectile
	UPROPERTY(EditAnywhere, Category = Projectile)
	UMaterialInterface* ProjectileMaterial;

	// Dynamic Material Instance for projectile
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	// Niagara particle system color property
	UPROPERTY(EditAnywhere, Category = Effects)
	UNiagaraSystem* ColorP;

public:
	AFirstPerson415Projectile();

protected:
	virtual void BeginPlay() override;

public:

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

