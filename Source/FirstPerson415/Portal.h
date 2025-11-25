// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Portal.generated.h"

class FirstPerson415Character;

UCLASS()
class FIRSTPERSON415_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The mesh representing the portal
	UPROPERTY(EditAnywhere, Category = "Portal")
	UStaticMeshComponent* PortalMesh;

	// The scene capture component for rendering the view through the portal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	USceneCaptureComponent2D* SceneCaptureComponent;

	// The arrow component indicating the exit direction of the portal
	UPROPERTY(EditAnywhere, Category = "Portal")
	UArrowComponent* PortalExit;

	// The render target where the portal view is rendered
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
	UTextureRenderTarget2D* RenderTarget;

	// The box component used as a trigger for teleportation
	UPROPERTY(EditAnywhere, Category = "Portal")
	UBoxComponent* PortalTrigger;

	// The linked portal that this portal connects to
	UPROPERTY(EditAnywhere, Category = "Portal")
	APortal* LinkedPortal;

	// The material applied to the portal surface
	UPROPERTY(EditAnywhere, Category = "Portal")
	UMaterialInterface* PortalMaterial;
	
	// Overlap begin function for teleportation
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Function to set the IsTeleporting boolean variable in the player character
	UFUNCTION()
	void SetBool(AFirstPerson415Character* PlayerChar);

	// Function to update the portal view each frame
	UFUNCTION()
	void UpdatePortalView();
};
