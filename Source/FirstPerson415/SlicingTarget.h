// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h" 
#include "SlicingTarget.generated.h"

UCLASS()
class FIRSTPERSON415_API ASlicingTarget : public AActor
{
	GENERATED_BODY()

public:
	ASlicingTarget();

protected:
	virtual void BeginPlay() override;

public:
	// The Source Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slicing")
	UStaticMeshComponent* StaticMeshComp;

	// The Destination Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slicing")
	UProceduralMeshComponent* ProcMeshComp;

	// Material for the inside of the cut
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slicing")
	UMaterialInterface* CapMaterial;

	// Function to perform the slice
	UFUNCTION(BlueprintCallable, Category = "Slicing")
	void Slice(FVector PlanePosition, FVector PlaneNormal);
};