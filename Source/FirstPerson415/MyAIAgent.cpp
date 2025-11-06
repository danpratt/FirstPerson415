// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAIAgent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "FirstPerson415Projectile.h"

// Sets default values
AMyAIAgent::AMyAIAgent()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup AI Perception Component and Sight Sense
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 2000.0f;
	SightConfig->LoseSightRadius = 2200.0f;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->SetMaxAge(0.5f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	// Setup GunMesh
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GetMesh(), FName("hand_r"));
}

// Called when the game starts or when spawned
void AMyAIAgent::BeginPlay()
{
	Super::BeginPlay();

	// Bind the OnTargetPerceptionUpdated function to the AI Perception Component
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AMyAIAgent::OnTargetPerceptionUpdated);
	}
}

void AMyAIAgent::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Actor == PlayerPawn && Stimulus.WasSuccessfullySensed())
	{
		if (GunMesh == nullptr || ProjectileClass == nullptr)
		{
			return;
		}

		// Get Start Location
		FVector StartLocation = GunMesh->GetSocketLocation("Muzzle");

		// Get Target Location
		// We'll get the Player's Camera location instead of their feet
		FVector TargetLocation = PlayerPawn->GetPawnViewLocation();

		// 3. Calculate the rotation
		FRotator AimRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);

		// 4. Set color and pass the AimRotation to the Fire() function
		DecalColor = FLinearColor::MakeRandomColor();
		Fire(AimRotation);
	}
}

void AMyAIAgent::Fire(FRotator AimRotation)
{
	// We already checked ProjectileClass and GunMesh in the other function,
	// but a check here is still good practice.
	if (ProjectileClass == nullptr || GunMesh == nullptr)
	{
		return;
	}

	// Get spawn location
	FVector SpawnLocation = GunMesh->GetSocketLocation("Muzzle");

	// Use the AimRotation we passed in as the SpawnRotation
	FRotator SpawnRotation = AimRotation;

	// Set up spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// Spawn the projectile
	GetWorld()->SpawnActor<AFirstPerson415Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}

// Called every frame
void AMyAIAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyAIAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

