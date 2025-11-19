// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinProcTerrain.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APerlinProcTerrain::APerlinProcTerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup attachments
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProcMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void APerlinProcTerrain::BeginPlay()
{
	Super::BeginPlay();
	
	// Generate the terrain
	CreateVertices();
	CreateTriangles();
	ProcMesh->CreateMeshSection(SectionID, 
		Vertices, 
		Triangles, 
		Normals, 
		UV0, 
		UpVertexColors, 
		TArray<FProcMeshTangent>(), 
		true);

	// Apply the material
	if (TerrainMaterial)
	{
		ProcMesh->SetMaterial(0, TerrainMaterial);
	}
}

// Called every frame
void APerlinProcTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APerlinProcTerrain::AlterMesh(FVector ImpactPoint)
{
	// Function to alter the mesh at the impact point
	for (int i = 0; i < Vertices.Num(); i++)
	{
		FVector TempVector = ImpactPoint - this->GetActorLocation();

		if (FVector(Vertices[i] - TempVector).Size() < Radius)
		{
			// Lower the vertex by Depth
			Vertices[i] = Vertices[i] - Depth;
			// Update the mesh section
			ProcMesh->UpdateMeshSection(
				SectionID, 
				Vertices, 
				Normals, 
				UV0, 
				UpVertexColors, 
				TArray<FProcMeshTangent>());
		}
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
			//GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z: %f"), Z));
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

