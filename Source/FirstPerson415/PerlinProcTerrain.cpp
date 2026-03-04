// Fill out your copyright notice in the Description page of Project Settings.

#include "PerlinProcTerrain.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Engine/Engine.h"

// Sets default values
APerlinProcTerrain::APerlinProcTerrain()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // Setup attachments
    ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    ProcMesh->SetupAttachment(GetRootComponent());

    // NOTE: HISM creation has been removed from here because we dynamically create them in GenerateFoliage()
}

// Called when the game starts or when spawned
void APerlinProcTerrain::BeginPlay()
{
    Super::BeginPlay();

    CreateVertices();
    CreateTriangles();

    // Calculate Normals so we can detect steep slopes for HISM foliage placement later
    TArray<FProcMeshTangent> Tangents;
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

    ProcMesh->CreateMeshSection(SectionID,
        Vertices,
        Triangles,
        Normals,
        UV0,
        UpVertexColors,
        Tangents, // Pass the calculated tangents here
        true);

    if (TerrainMaterial)
    {
        ProcMesh->SetMaterial(0, TerrainMaterial);
    }

    // Spawn the foliage after the mesh is generated so we can use the vertex data for placement
    GenerateFoliage();
}

// Called every frame
void APerlinProcTerrain::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APerlinProcTerrain::AlterMesh(FVector ImpactPoint)
{
    // Local variable to track if we actually changed anything
    bool bHasChanged = false;

    // Function to alter the mesh at the impact point
    for (int i = 0; i < Vertices.Num(); i++)
    {
        FVector TempVector = ImpactPoint - this->GetActorLocation();

        if (FVector(Vertices[i] - TempVector).Size() < Radius)
        {
            // Lower the vertex by Depth
            Vertices[i] = Vertices[i] - Depth;

            // Mark that we made a change
            bHasChanged = true;
        }
    }

    // Update the mesh ONLY ONCE, and ONLY if we changed something ---
    if (bHasChanged)
    {
        ProcMesh->UpdateMeshSection(
            SectionID,
            Vertices,
            Normals,
            UV0,
            UpVertexColors,
            TArray<FProcMeshTangent>());
    }
}

void APerlinProcTerrain::CreateVertices()
{
    // Create vertices based on terrain dimensions and Perlin noise
    for (int x = 0; x <= TerrainWidth; x++)
    {
        for (int y = 0; y <= TerrainLength; y++)
        {
            // Calculate Z value using Perlin noise
            float Z = FMath::PerlinNoise2D(FVector2D(x * NoiseScale + 0.1,
                y * NoiseScale + 0.1)) * NoiseMultiplier;

            Vertices.Add(FVector(x * TerrainScale, y * TerrainScale, Z));
            UV0.Add(FVector2D(x * UVScale, y * UVScale));
        }
    }
}

void APerlinProcTerrain::CreateTriangles()
{
    // Create triangles based on vertices
    int Vertex = 0;  // Current vertex index

    for (int x = 0; x < TerrainWidth; x++)
    {
        for (int y = 0; y < TerrainLength; y++)
        {
            // First triangle
            Triangles.Add(Vertex);
            Triangles.Add(Vertex + 1);
            Triangles.Add(Vertex + TerrainLength + 1);
            // Second triangle
            Triangles.Add(Vertex + 1);
            Triangles.Add(Vertex + TerrainLength + 2);
            Triangles.Add(Vertex + TerrainLength + 1);
            Vertex++;
        }
        Vertex++;
    }
}

void APerlinProcTerrain::GenerateFoliage()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Generating Foliage..."));

    // Destroy old foliage components if this is called multiple times (like from the UI slider)
    for (UHierarchicalInstancedStaticMeshComponent* HISM : ActiveHISMs)
    {
        if (HISM)
        {
            HISM->DestroyComponent();
        }
    }
    ActiveHISMs.Empty();

    // Loop through every foliage type you added in the Blueprint Details Panel
    for (const FFoliageSetup& Setup : FoliageTypes)
    {
        // Skip if you forgot to assign a mesh
        if (!Setup.FoliageMesh) continue;

        // Dynamically create a new HISM component for this specific mesh
        UHierarchicalInstancedStaticMeshComponent* NewHISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
        NewHISM->SetStaticMesh(Setup.FoliageMesh);
        NewHISM->SetupAttachment(ProcMesh);
        NewHISM->RegisterComponent(); // Required when creating components outside the constructor

        // Disable all collisions for performance and player movement
        NewHISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        ActiveHISMs.Add(NewHISM);

        // Calculate the threshold for the slope math
        float SlopeThreshold = FMath::Cos(FMath::DegreesToRadians(Setup.MaxSlopeAngle));

        // Spawn the instances for this specific foliage type
        for (int32 i = 0; i < Vertices.Num(); ++i)
        {
            // Density Check
            if (FMath::FRand() > (Setup.Density * GlobalDensityMultiplier)) continue;

            FVector CurrentLocation = Vertices[i];
            FVector CurrentNormal = Normals[i];

            // Elevation Check
            if (CurrentLocation.Z < Setup.MinElevation || CurrentLocation.Z > Setup.MaxElevation) continue;

            // Slope Check
            float SlopeDot = FVector::DotProduct(CurrentNormal, FVector::UpVector);
            if (SlopeDot < SlopeThreshold) continue;

            // Generate random rotation and scale based on the struct settings
            FRotator RandomRotation = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f);
            FVector RandomScale = FVector(FMath::RandRange(Setup.MinScale, Setup.MaxScale));

            // Create the transform matrix and add the instance
            FTransform InstanceTransform(RandomRotation, CurrentLocation, RandomScale);
            NewHISM->AddInstance(InstanceTransform);
        }
    }
}