// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcPlane.h"
#include "ProceduralMeshComponent.h"
#include "ProcMeshFromStatic.h"

// Sets default values
AProcPlane::AProcPlane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create Procedural Mesh Component
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
}

// Called when the game starts or when spawned
void AProcPlane::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProcPlane::PostActorCreated()
{
	Super::PostActorCreated();
	CreateMesh();
}

void AProcPlane::PostLoad()
{
	Super::PostLoad();
	CreateMesh();
}

// Called every frame
void AProcPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProcPlane::CreateMesh()
{
	// Create the mesh section
	ProcMesh->CreateMeshSection(
		0, 
		Vertices, 
		Triangles, 
		TArray<FVector>(), 
		UV0, 
		TArray<FColor>(), 
		TArray<FProcMeshTangent>(), 
		true);

	// Set material if assigned
	if (ProcMesh && PlaneMat)
	{
		ProcMesh->SetMaterial(0, PlaneMat);
	}
}

