// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcMeshFromStatic.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
AProcMeshFromStatic::AProcMeshFromStatic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create default subjects
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));

	// Setup Attachments
	RootComponent = ProcMesh;
	BaseMesh->SetupAttachment(ProcMesh);
}

// Called when the game starts or when spawned
void AProcMeshFromStatic::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProcMeshFromStatic::PostActorCreated()
{
	Super::PostActorCreated();
	GetMeshData();
}

void AProcMeshFromStatic::PostLoad()
{
	Super::PostLoad();
	GetMeshData();
}

// Called every frame
void AProcMeshFromStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProcMeshFromStatic::GetMeshData()
{
	// Make sure we have a valid Static Mesh
	if (BaseMesh)
	{
		// Get the Static Mesh from the Static Mesh Component
		UStaticMesh* StaticMesh = BaseMesh->GetStaticMesh();

		// If Static Mesh is valid, extract the mesh data
		if (StaticMesh)
		{
			// Get mesh section data from LOD 0, Section 0
			UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(
				StaticMesh,
				0,
				0,
				Vertices,
				Triangles,
				Normals,
				UV0,
				Tangents);
			
			// Take extracted mesh data and update the Procedural Mesh Component
			ProcMesh->UpdateMeshSection(
				0,
				Vertices,
				Normals,
				UV0,
				UpVertexColors,
				Tangents);

			CreateMesh();
		}
	}
}

void AProcMeshFromStatic::CreateMesh()
{
	// Make sure we have a valid Static Mesh
	if (BaseMesh) {
		// Create the mesh section
		ProcMesh->CreateMeshSection(
			0,
			Vertices,
			Triangles,
			Normals,
			UV0,
			UpVertexColors,
			Tangents,
			true);
	}
}

