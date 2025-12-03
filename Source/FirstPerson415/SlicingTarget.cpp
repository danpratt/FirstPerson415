// Fill out your copyright notice in the Description page of Project Settings.

#include "SlicingTarget.h"
#include "KismetProceduralMeshLibrary.h"
#include "Components/StaticMeshComponent.h"

ASlicingTarget::ASlicingTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	// Setup Root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Setup Source Mesh (Standard Cube)
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(RootComponent);
	// We disable collision on the source because the ProcMesh handles it
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Setup Proc Mesh (The sliceable one)
	ProcMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	ProcMeshComp->SetupAttachment(RootComponent);

	// Allow physics simulation
	ProcMeshComp->SetSimulatePhysics(true);
	ProcMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	ProcMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProcMeshComp->bUseComplexAsSimpleCollision = false;
}

void ASlicingTarget::BeginPlay()
{
	Super::BeginPlay();

	// COPY THE MESH
	// This takes whatever mesh assigned in the Blueprint
	// and copies it into the Procedural Mesh.
	if (StaticMeshComp->GetStaticMesh() != nullptr)
	{
		UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(
			StaticMeshComp,
			0,
			ProcMeshComp,
			true
		);
	}

	// Hide the Source
	// We only want to see the sliceable version
	StaticMeshComp->SetHiddenInGame(true);
}

void ASlicingTarget::Slice(FVector PlanePosition, FVector PlaneNormal)
{
	// Create "Other Half" Component
	UProceduralMeshComponent* OtherHalf = NewObject<UProceduralMeshComponent>(this);
	OtherHalf->RegisterComponent();
	OtherHalf->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	OtherHalf->SetWorldLocation(ProcMeshComp->GetComponentLocation());
	OtherHalf->SetWorldRotation(ProcMeshComp->GetComponentRotation());

	// Copy materials
	OtherHalf->SetMaterial(0, ProcMeshComp->GetMaterial(0));
	OtherHalf->SetMaterial(1, ProcMeshComp->GetMaterial(1));

	// Perform Slice
	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		ProcMeshComp,
		PlanePosition,
		PlaneNormal,
		true,
		OtherHalf,
		EProcMeshSliceCapOption::CreateNewSectionForCap,
		CapMaterial
	);

	ProcMeshComp->bUseComplexAsSimpleCollision = false;
	ProcMeshComp->ClearCollisionConvexMeshes(); // Remove old box shape

	FProcMeshSection* Section = ProcMeshComp->GetProcMeshSection(0);
	if (Section)
	{
		TArray<FVector> CollisionVertices;
		for (const FProcMeshVertex& Vert : Section->ProcVertexBuffer)
		{
			CollisionVertices.Add(Vert.Position);
		}
		ProcMeshComp->AddCollisionConvexMesh(CollisionVertices);
	}

	OtherHalf->bUseComplexAsSimpleCollision = false;
	OtherHalf->ClearCollisionConvexMeshes();

	FProcMeshSection* OtherSection = OtherHalf->GetProcMeshSection(0);
	if (OtherSection)
	{
		TArray<FVector> CollisionVertices;
		for (const FProcMeshVertex& Vert : OtherSection->ProcVertexBuffer)
		{
			CollisionVertices.Add(Vert.Position);
		}
		OtherHalf->AddCollisionConvexMesh(CollisionVertices);
	}

	// Enable Physics (Now that shapes exist)
	ProcMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProcMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	ProcMeshComp->SetSimulatePhysics(true);

	OtherHalf->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OtherHalf->SetCollisionObjectType(ECC_PhysicsBody);
	OtherHalf->SetSimulatePhysics(true);

	// Apply Impulse
	FVector Impulse = PlaneNormal * 500.0f;
	ProcMeshComp->AddImpulse(Impulse, NAME_None, true);
	OtherHalf->AddImpulse(-Impulse, NAME_None, true);
}