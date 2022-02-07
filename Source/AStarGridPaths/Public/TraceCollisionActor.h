// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TraceCollisionActor.generated.h"

UCLASS()
class ASTARGRIDPATHS_API ATraceCollisionActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATraceCollisionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupGridMaterial();

	/*
		Mainly used to as a collision mesh for our trace from camera to get mouse to world location.
		We scale this up and also have a visual rep of the grid extents
	*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	UStaticMeshComponent* CellMesh;

};
