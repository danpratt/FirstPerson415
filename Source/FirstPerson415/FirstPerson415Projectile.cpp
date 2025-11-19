// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPerson415Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PerlinProcTerrain.h"

AFirstPerson415Projectile::AFirstPerson415Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFirstPerson415Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Setup Ball Mesh
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>("BallMesh");

	// Set as root component
	RootComponent = CollisionComp;

	BallMesh->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AFirstPerson415Projectile::BeginPlay()
{
	Super::BeginPlay();

	// Generate a random color; this will be used for both the projectile and the decal
	float red = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
	float green = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
	float blue = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
	RandomColor = FLinearColor(red, green, blue, 1.0f);

	// Make sure we have valid material and mesh before trying to create dynamic material
	if (ProjectileMaterial == nullptr || BallMesh == nullptr)
	{
		return;
	}

	// Create dynamic material instance and set the color parameter
	DynamicMaterial = UMaterialInstanceDynamic::Create(ProjectileMaterial, this);
	BallMesh->SetMaterial(0, DynamicMaterial);
	DynamicMaterial->SetVectorParameterValue(FName(TEXT("Color")), RandomColor);
}

void AFirstPerson415Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Defensive checks common to all paths
	if (OtherActor == nullptr)
	{
		return;
	}

	// Generate random frame and decal size
	const float Frame = FMath::FRandRange(0.0f, 3.0f);
	const FVector DecalSize = FVector(FMath::FRandRange(20.0f, 40.0f));

	// Ensure we have a valid world and material before trying to spawn a decal
	UWorld* World = GetWorld();
	if (!World || SplatMaterial == nullptr)
	{
		// If we hit a physics object still apply impulse, then destroy if appropriate
		if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
			Destroy();
		}
		return;
	}

	// If we hit something with physics, apply impulse first
	if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	// Spawn niagara particle system at hit location
	if (ColorP != nullptr)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			ColorP,
			HitComp,
			NAME_None,
			FVector(-20.f, 0.f, 0.f),
			FRotator(0.f),
			EAttachLocation::KeepRelativeOffset,
			true
		);

		if (NiagaraComp != nullptr)
		{
			NiagaraComp->SetVariableLinearColor(FName("RandColor"), RandomColor);
			BallMesh->DestroyComponent();
			CollisionComp->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	// Spawn the decal only after we've validated BallMaterial and World
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
		World,
		SplatMaterial,
		DecalSize,
		Hit.ImpactPoint,
		Hit.ImpactNormal.Rotation(),
		0.0f
	);

	// Check spawn success
	if (Decal != nullptr)
	{
		// Create dynamic material instance for decal
		UMaterialInstanceDynamic* MatInstance = Decal->CreateDynamicMaterialInstance();
		if (MatInstance != nullptr)
		{
			// Set the decal color and frame parameters
			MatInstance->SetVectorParameterValue(FName(TEXT("Color")), RandomColor);
			MatInstance->SetScalarParameterValue(FName(TEXT("Frame")), Frame);

			APerlinProcTerrain* Terrain = Cast<APerlinProcTerrain>(OtherActor);

			if (Terrain)
			{
				// If we hit the procedural terrain, alter the mesh at the impact point
				Terrain->AlterMesh(Hit.ImpactPoint);
			}
		}
	}

	// If we hit a physics-simulating component we want to destroy the projectile.
	// Do this after decal spawn so we don't access a partially-destroyed actor while spawning.
	if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
	{
		Destroy();
	}
}