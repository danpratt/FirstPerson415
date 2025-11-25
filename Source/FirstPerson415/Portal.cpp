// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "FirstPerson415Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PortalTrigger"));
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	PortalExit = CreateDefaultSubobject<UArrowComponent>(TEXT("PortalExit"));

	// Set up component hierarchy
	RootComponent = PortalTrigger;
	PortalMesh->SetupAttachment(PortalTrigger);
	SceneCaptureComponent->SetupAttachment(PortalMesh);
	PortalExit->SetupAttachment(PortalTrigger);

	// Turn off collisions for the portal mesh (handled by the trigger overlap event function)
	PortalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	// Bind the overlap event
	PortalTrigger->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
	PortalMesh->SetHiddenInSceneCapture(true);
	PortalMesh->CastShadow = false;
	if (PortalMaterial)
	{
		PortalMesh->SetMaterial(0, PortalMaterial);
	}

}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePortalView();
}

// Overlap begin function for teleportation
void APortal::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AFirstPerson415Character* PlayerChar = Cast<AFirstPerson415Character>(OtherActor);

	// Teleport the player to the linked portal's location if not already teleporting

	// Verify PlayerChar and LinkedPortal are valid
	if (PlayerChar && LinkedPortal)
	{
		// Check if the player is not already teleporting
		if (!PlayerChar->IsTeleporting)
		{
			// Teleport the player to the linked portal's location
			PlayerChar->IsTeleporting = true;
			FVector Loc = LinkedPortal->PortalExit->GetComponentLocation();
			PlayerChar->SetActorLocation(Loc);

			// Set a timer to reset the IsTeleporting flag after a short delay
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, FName("SetBool"), PlayerChar);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.f, false);
		}
	}
}

// Function to set the IsTeleporting boolean variable in the player character
void APortal::SetBool(AFirstPerson415Character* PlayerChar)
{
	// Verify PlayerChar is valid
	if (PlayerChar)
	{
		// Reset the IsTeleporting flag
		PlayerChar->IsTeleporting = false;
	}
}

// Function to update the portal view each frame
void APortal::UpdatePortalView()
{
	// Get the relative location and rotation of the player camera to the linked portal
	FVector Location = this->GetActorLocation() - LinkedPortal->GetActorLocation();
	// Get player camera location and rotation
	FVector PlayerCamLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)
		->GetTransformComponent()->GetComponentLocation();
	FRotator PlayerCamRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)
		->GetTransformComponent()->GetComponentRotation();

	// Calculate new location and rotation for the scene capture component
	FVector CombinedLocation = PlayerCamLocation + Location;

	// Set the scene capture component's location and rotation
	SceneCaptureComponent->SetWorldLocationAndRotation(CombinedLocation, PlayerCamRotation);
}

