// Fill out your copyright notice in the Description page of Project Settings.

#include "SlicingTarget.h"
#include "KismetProceduralMeshLibrary.h" // Required for slicing and box generation
#include "TimerManager.h" // Add at top with other includes

// In the constructor, stop enabling simulation immediately so physics doesn't start
// before world/terrain collision is ready.
ASlicingTarget::ASlicingTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. Setup the Procedural Mesh
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	RootComponent = ProcMesh;

	// Enable physics so the target (and its slices) can fall
	ProcMesh->bUseComplexAsSimpleCollision = false; // Required for physics on proc meshes

	// Do NOT enable simulate physics here. Enabling in constructor can start simulation
	// before other actors (terrain) finish creating collision and cause objects to fall through.
	// ProcMesh->SetSimulatePhysics(true);

	ProcMesh->SetCollisionObjectType(ECC_PhysicsBody);
}

void ASlicingTarget::BeginPlay()
{
	Super::BeginPlay();

	// 1. SET COLLISION PROPERTIES FIRST (before creating mesh!)
	ProcMesh->SetMobility(EComponentMobility::Movable);
	ProcMesh->bUseComplexAsSimpleCollision = false;
	ProcMesh->SetCollisionObjectType(ECC_PhysicsBody);
	ProcMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 2. Generate the Box Mesh data
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;

	UKismetProceduralMeshLibrary::GenerateBoxMesh(FVector(50.f, 50.f, 50.f), Vertices, Triangles, Normals, UVs, Tangents);

	// 3. Create the mesh section (With Collision = true)
	ProcMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, true);

	// 4. CRITICAL: Recreate physics state immediately after mesh creation
	ProcMesh->RecreatePhysicsState();

	// 5. Delay enabling simulation so terrain collision is ready
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(EnablePhysicsTimerHandle, this, &ASlicingTarget::EnablePhysics, 0.25f, false);
	}
	else
	{
		EnablePhysics();
	}
}

void ASlicingTarget::EnablePhysics()
{
	if (!ProcMesh)
	{
		return;
	}

	// Enable simulation now that collision should be ready
	ProcMesh->SetSimulatePhysics(true);
	ProcMesh->WakeAllRigidBodies();
}

void ASlicingTarget::Slice(FVector PlanePosition, FVector PlaneNormal)
{
	// Create a new component to hold the "other half" of the slice
	UProceduralMeshComponent* OtherHalf = NewObject<UProceduralMeshComponent>(this);

	// Attach and register
	OtherHalf->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	OtherHalf->RegisterComponent();

	// Make sure it has the right mobility and collision so physics will see it
	OtherHalf->SetMobility(EComponentMobility::Movable);
	OtherHalf->bUseComplexAsSimpleCollision = false;
	OtherHalf->SetCollisionProfileName(TEXT("PhysicsActor"));
	OtherHalf->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OtherHalf->SetCollisionObjectType(ECC_PhysicsBody);

	OtherHalf->SetWorldLocation(ProcMesh->GetComponentLocation());
	OtherHalf->SetWorldRotation(ProcMesh->GetComponentRotation());

	// Perform the Slice (keeps one half in ProcMesh and fills OtherHalf)
	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		ProcMesh,
		PlanePosition,
		PlaneNormal,
		true,
		OtherHalf,
		EProcMeshSliceCapOption::CreateNewSectionForCap,
		CapMaterial
	);

	// Recreate physics state for the newly-filled OtherHalf so collision bodies exist
	OtherHalf->RecreatePhysicsState();
	OtherHalf->SetSimulatePhysics(true);
	OtherHalf->WakeAllRigidBodies();

	// Ensure original piece has up-to-date physics body and simulation
	ProcMesh->RecreatePhysicsState();
	ProcMesh->SetSimulatePhysics(true);
	ProcMesh->WakeAllRigidBodies();

	// Optional: Add a little impulse
	FVector ImpulseDir = PlaneNormal * 500.0f;
	ProcMesh->AddImpulse(ImpulseDir, NAME_None, true);
	OtherHalf->AddImpulse(-ImpulseDir, NAME_None, true);
}