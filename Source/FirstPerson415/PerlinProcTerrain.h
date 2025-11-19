// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PerlinProcTerrain.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

UCLASS()
class FIRSTPERSON415_API APerlinProcTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APerlinProcTerrain();

	// Bounds for terrain generation
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Terrain")
	int TerrainWidth = 0;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0), Category = "Terrain")
	int TerrainLength = 0;

	// Multiplier and Scale of the Perlin noise
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.f), Category = "Terrain")
	float NoiseMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.f), Category = "Terrain")
	float NoiseScale = 1.0f;

	// Scaling factor for the terrain mesh
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001f), Category = "Terrain")
	float TerrainScale = 0.f;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001f), Category = "Terrain")
	float UVScale = 0.f;

	// Radius of hole to be generated
	UPROPERTY(EditAnywhere, Category = "Terrain")
	float Radius;

	// Depth of hole to be generated
	UPROPERTY(EditAnywhere, Category = "Terrain")
	FVector Depth;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Material for the procedural mesh
	UPROPERTY(EditAnywhere, Category = "Terrain")
	UMaterialInterface* TerrainMaterial;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function to alter the mesh at the impact point
	UFUNCTION()
	void AlterMesh(FVector ImpactPoint);

private:
	// Procedural Mesh Component
	UProceduralMeshComponent* ProcMesh;

	// Mesh components
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FColor> UpVertexColors;

	// Section ID for the procedural mesh (note: not mutated in current implementation)
	int SectionID = 0;

	// Generation functions
	void CreateVertices();
	void CreateTriangles();
};
