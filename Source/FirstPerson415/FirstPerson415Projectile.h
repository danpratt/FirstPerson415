// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FirstPerson415Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

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

	// Projectile Mesh
	UPROPERTY(EditAnywhere, Category = Projectile)
	UStaticMeshComponent* BallMesh;

	UPROPERTY(EditAnywhere, Category = Projectile)
	UMaterial* BallMaterial;

	UPROPERTY(EditAnywhere, Category = Projectile)
	FLinearColor RandomColor;

	UPROPERTY(EditAnywhere, Category = Projectile)
	UMaterialInterface* ProjectileMaterial;

	UPROPERTY(EditAnywhere, Category = Projectile)
	UMaterialInstanceDynamic* DynamicMaterial;

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

