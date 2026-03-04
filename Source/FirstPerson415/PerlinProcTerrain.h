// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "PerlinProcTerrain.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

// Foliage Struct That will be used to set up the different types of foliage in the editor (trees, grass, ferns)
// and then used to generate them in the world based on the terrain mesh data
USTRUCT(BlueprintType)
struct FFoliageSetup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
    UStaticMesh* FoliageMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Density = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
    float MinElevation = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
    float MaxElevation = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
    float MaxSlopeAngle = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
    float MinScale = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Settings")
    float MaxScale = 1.2f;
};

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

    // --- Foliage Generation ---
public:
    // Array to hold all the different types of foliage (trees, grass, ferns) setup in the editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage")
    TArray<FFoliageSetup> FoliageTypes;

    // Array to track dynamically spawned HISM components so we can clear them easily
    UPROPERTY()
    TArray<UHierarchicalInstancedStaticMeshComponent*> ActiveHISMs;

    // Master multiplier controlled by the UI Slider
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage")
    float GlobalDensityMultiplier = 1.0f;

    // Function to calculate and spawn the trees
    UFUNCTION(BlueprintCallable, Category = "Foliage")
    void GenerateFoliage();
};