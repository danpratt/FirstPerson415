// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProcPlane.generated.h"

class UProceduralMeshComponent;

UCLASS()
class FIRSTPERSON415_API AProcPlane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProcPlane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Additional lifecycle hooks
	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Procedural Mesh Component

	// Vertices to draw
	UPROPERTY(EditAnywhere, Category = "Components")
	TArray<FVector> Vertices;

	// Order to draw triangles
	UPROPERTY(EditAnywhere, Category = "Components")
	TArray<int> Triangles;

	// UV Setup
	UPROPERTY(EditAnywhere, Category = "Components")
	TArray<FVector2D> UV0;

	// Material for the plane
	UPROPERTY(EditAnywhere, Category = "Components")
	UMaterialInterface* PlaneMat;

	// Function to create the mesh
	UFUNCTION()
	void CreateMesh();

private:
	UProceduralMeshComponent* ProcMesh;
};
