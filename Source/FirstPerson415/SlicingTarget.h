// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h" // Required for the component type
#include "SlicingTarget.generated.h"

UCLASS()
class FIRSTPERSON415_API ASlicingTarget : public AActor
{
	GENERATED_BODY()

public:
	ASlicingTarget();

protected:
	virtual void BeginPlay() override;

public:
	// The main procedural mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slicing")
	UProceduralMeshComponent* ProcMesh;

	// The material to apply to the "inside" of the cut
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slicing")
	UMaterialInterface* CapMaterial;

	// The function to call when we want to slice this target
	// BlueprintCallable allows us to call this easily from our Projectile logic
	UFUNCTION(BlueprintCallable, Category = "Slicing")
	void Slice(FVector PlanePosition, FVector PlaneNormal);

private:
	FTimerHandle EnablePhysicsTimerHandle;
	void EnablePhysics();
};