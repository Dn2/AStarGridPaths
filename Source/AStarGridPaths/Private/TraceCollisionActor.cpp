// Fill out your copyright notice in the Description page of Project Settings.


#include "TraceCollisionActor.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

// Sets default values
ATraceCollisionActor::ATraceCollisionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	

	CellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = CellMesh;
	CellMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	CellMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	//CellMesh->OnClicked

	//FString testPath = IPluginManager::Get().FindPlugin("AStarGridPaths")->GetMountedAssetPath();
	FString meshPath = "StaticMesh'" + IPluginManager::Get().FindPlugin("AStarGridPaths")->GetMountedAssetPath() + "StaticMeshes/Gridx100.Gridx100'";
	FString matPath = "Material'" + IPluginManager::Get().FindPlugin("AStarGridPaths")->GetMountedAssetPath() + "Materials/GridChecker_Mat.GridChecker_Mat'";
	//UE_LOG(LogTemp, Warning, TEXT("DIR: %s"), *testPath);

	//TCHAR* meshChar = meshPath;
	//TEXT("StaticMesh'/AStarGridPaths/StaticMeshes/Gridx100.Gridx100'")

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*meshPath);
	UStaticMesh* Asset = MeshAsset.Object;
	CellMesh->SetStaticMesh(Asset);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>MatAsset(*matPath);
	UMaterialInterface* Material = MatAsset.Object;
	CellMesh->SetMaterial(0, Material);/**/

	//StaticMesh'/AStarGridPaths/StaticMeshes/Gridx100.Gridx100'
}

// Called when the game starts or when spawned
void ATraceCollisionActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATraceCollisionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATraceCollisionActor::SetupGridMaterial()
{
	if (CellMesh)
	{
		//Set grid size in our material
		UMaterialInterface* Material = CellMesh->GetMaterial(0);
		UMaterialInstanceDynamic* GridMI = CellMesh->CreateDynamicMaterialInstance(0, Material);
	}
}

