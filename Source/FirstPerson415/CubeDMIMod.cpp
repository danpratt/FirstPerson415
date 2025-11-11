// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeDMIMod.h"
#include "FirstPerson415Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
ACubeDMIMod::ACubeDMIMod()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");

	RootComponent = BoxComponent;
	CubeMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACubeDMIMod::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACubeDMIMod::OnOverlapBegin);

	if (BaseMaterial)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (DynamicMaterialInstance && CubeMesh)
		{
			CubeMesh->SetMaterial(0, DynamicMaterialInstance);
		}
	}
}

// Called every frame
void ACubeDMIMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeDMIMod::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFirstPerson415Character* Character = Cast<AFirstPerson415Character>(OtherActor);

	if (Character && DynamicMaterialInstance)
	{
		float red = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
		float green = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
		float blue = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);

		FLinearColor RandomColor = FLinearColor(red, green, blue, 1.0f);
		DynamicMaterialInstance->SetVectorParameterValue(FName("Color"), RandomColor);
		DynamicMaterialInstance->SetScalarParameterValue(FName("Darkness"), UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f));

		if (ColorP == nullptr || OtherComp == nullptr)
		{
			return;
		}

		// Set color for Niagara particle system
		UNiagaraComponent* ParticleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			ColorP,
			OtherComp,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
		if (ParticleComp)
		{
			ParticleComp->SetVariableLinearColor(FName("User.RandColor"), RandomColor);
		}
	}
}

