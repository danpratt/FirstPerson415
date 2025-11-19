// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProcMeshFromStatic.generated.h"

UCLASS()
class FIRSTPERSON415_API AProcMeshFromStatic : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProcMeshFromStatic();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Additional lifecycle functions
	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Procedural Mesh Component

	// Vertices Array
	UPROPERTY()
	TArray<FVector> Vertices;

	// Triangles Array
	UPROPERTY()
	TArray<int> Triangles;

	// Normals Array
	UPROPERTY()
	TArray<FVector> Normals;

	// UVs Array
	UPROPERTY()
	TArray<FVector2D> UV0;

	// Colors Arrays
	UPROPERTY()
	TArray<FLinearColor> VertexColors;

	UPROPERTY()
	TArray<FColor> UpVertexColors;

	// Tangents Array
	UPROPERTY()
	TArray<FProcMeshTangent> Tangents;

	// Base Mesh to copy from
	UPROPERTY(EditAnywhere, Category="Component")
	UStaticMeshComponent* BaseMesh;

private:
	UProceduralMeshComponent* ProcMesh;
	void GetMeshData();
	void CreateMesh();
};
