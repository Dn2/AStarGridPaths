// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarGISubsystem.h"
#include "Engine/World.h"
#include "Async/AsyncWork.h"
#include "Async/Async.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
//#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
//#include "TraceCollisionActor.h"
//#include "Engine/Classes/Materials/MaterialInterface.h"

void UAStarGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	/**
	 *	Set up our default grid.
	 */

	
	//always ensures positive values.
	DefaultGridExtents.X = FMath::Abs(DefaultGridExtents.X);
	DefaultGridExtents.Y = FMath::Abs(DefaultGridExtents.Y);

	GridExtents.X = FMath::Abs(GridExtents.X);
	GridExtents.Y = FMath::Abs(GridExtents.Y);

	DefaultCellSize = FMath::Abs(DefaultCellSize);
	CellSize = FMath::Abs(CellSize);



	//If no values are set, get them from our class defaults.
	if (GridExtents.X < 1 || GridExtents.Y < 1)
	{
		GridExtents = DefaultGridExtents;
	}

	if (CellSize <= 1)
	{
		CellSize = DefaultCellSize;
	}

	if (!TraceCollisionBase)
	{
		//Spawn TraceCollisionActor so our trace has something to block it and we have some visual of grid size
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoFail = true;
		//SpawnInfo.Owner = GetGameInstance();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		TraceCollisionBase = GetWorld()->SpawnActor<ATraceCollisionActor>(ATraceCollisionActor::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), SpawnInfo);
		UE_LOG(LogTemp, Warning, TEXT("Trace Collision Actor Created!"));
	}


	OnWorldGridCreated_Internal.AddDynamic(this, &UAStarGISubsystem::OnGridCreated_Internal);
	OnWorldGridCreated.AddDynamic(this, &UAStarGISubsystem::OnGridCreated);

	OnAStarSearch.AddDynamic(this, &UAStarGISubsystem::AStarSearchToGoal_Internal);

}

void UAStarGISubsystem::Deinitialize()
{

}


TArray<FGridCellInfo> UAStarGISubsystem::CreateEmptyGridFloor(int32 XExtent, int32 YExtent, float Size) const
{
	//UE_LOG(LogTemp, Warning, TEXT("CellSize: %f"), Size);

	TArray<FGridCellInfo> GridArray;

	if (XExtent < 1 || YExtent < 1)
	{
		return GridArray;
	}

	/*
	Zero'd out indexes for the cell creation loop.
	X = colums, Y = rows, Index = the index of the cell in the cell array (CellArray).
	*/
	int32 X = 0;
	int32 Y = 0;
	int32 Index = 0;

	/*
		Creating our grid...
		Where Y = number of rows and X = number of columns.
		So while Y is less than the number of rows we need (GridExtents.Y),
		keep adding cells with the appropriate addresses.
		Ends when we run out of rows.
	*/
	while (Y < YExtent)
	{
		// Create a cell info with current address, location and index.
		GridArray.Add(FGridCellInfo(FIntPoint(X, Y), FVector(X*Size + (Size / 2), Y*Size + (Size / 2), 0), Index));

		//UE_LOG(LogTemp, Warning, TEXT("Creating: %s | Location: %s | Index: %d"), *FVector2D(X, Y).ToString(), *FVector(X*CellSize + (CellSize / 2), Y*CellSize + (CellSize / 2), 0).ToCompactString(), Index);

		// Increment the index for the next cell.
		++Index;

		// If we reach the end of GridExtent.X we reset X to 0 and increment Y,
		// moving us onto the next row of cells to start again
		if (X == XExtent - 1)
		{
			X = 0;
			++Y;
		}
		//Else just increment upon a single column one cell
		else
		{
			++X;
		}
	}

	return GridArray;
}

void UAStarGISubsystem::CreateEmptyWorldArray(FIntPoint Extents, float Size)
{
	//If no values are set, get them from our class defaults.
	if (Extents.X < 2 || Extents.Y < 2)
	{
		Extents = DefaultGridExtents;
	}
	if (Size <= 10.0f)
	{
		Size = DefaultCellSize;
	}

	CellSize = Size;
	CellArray = CreateEmptyGridFloor(Extents.X, Extents.Y, Size);
}

void UAStarGISubsystem::AsyncCreateEmptyWorldArray(FIntPoint Extents, float Size)
{
	//If no values are set, get them from our class defaults.
	if (Extents.X < 2 || Extents.Y < 2)
	{
		Extents = DefaultGridExtents;
	}
	if (Size <= 10.0f)
	{
		Size = DefaultCellSize;
	}

	CellSize = Size;

	//UE_LOG(LogTemp, Warning, TEXT("Ran| Extends: %d, %d | Size: %f"), Extents.X, Extents.Y, CellSize);
	//UE_LOG(LogTemp, Warning, TEXT("Extends: %d, %d"), Extents.X, Extents.Y);
	
	//TODO: set cellsize before running async task below. Also make sure its 
	(new FAutoDeleteAsyncTask<FCreateEmptyWorldArrayTask>(this, Extents, Size, OnWorldGridCreated))->StartBackgroundTask();
}

void UAStarGISubsystem::SetupTraceCollisionActor()
{
	if (GetWorld())
	{
		if (!TraceCollisionBase)
		{
			//Spawn TraceCollisionActor so our trace has something to block it and we have some visual of grid size
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoFail = true;
			//SpawnInfo.Owner = GetGameInstance();
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			TraceCollisionBase = GetWorld()->SpawnActor<ATraceCollisionActor>(ATraceCollisionActor::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), SpawnInfo);
			UE_LOG(LogTemp, Warning, TEXT("Trace Collision Actor Created!"));
		}


		if (TraceCollisionBase && TraceCollisionBase->CellMesh)
		{
			//TraceCollisionBase->SetupGridMaterial();
			//Set grid size in our material
			UMaterialInterface* Material = TraceCollisionBase->CellMesh->GetMaterial(0);
			UMaterialInstanceDynamic* GridMI = TraceCollisionBase->CellMesh->CreateDynamicMaterialInstance(0, Material);
			if (GridMI)
			{
				GridMI->SetScalarParameterValue(FName("GridSize"), CellSize);
				TraceCollisionBase->CellMesh->SetMaterial(0, GridMI);
			}/**/


			//Set up component for trace collisions
			FVector MeshScale;

			MeshScale.X = (CellSize*0.01f)*GridExtents.X;
			MeshScale.Y = (CellSize*0.01f)*GridExtents.Y;
			MeshScale.Z = 1;

			TraceCollisionBase->CellMesh->SetWorldScale3D(MeshScale);

			MeshScale.X = (GridExtents.X * CellSize)*0.5f;
			MeshScale.Y = (GridExtents.Y * CellSize)*0.5f;
			MeshScale.Z = 0;
			//TraceCollisionBase->CellMesh->SetWorldLocation(MeshScale * CellSize / 2);
			TraceCollisionBase->CellMesh->SetWorldLocation(MeshScale);
			
			//set up fog meshes
			UE_LOG(LogTemp, Warning, TEXT("Trace Collision Actor Setup done!"));
		}
	}
}



void UAStarGISubsystem::AutoSetupControllerEvents()
{
	if (TraceCollisionBase)
	{
		//bind onclick event from trace collision base actor
		TraceCollisionBase->CellMesh->OnClicked.AddDynamic(this, &UAStarGISubsystem::OnTraceBaseClicked);
	}

	if (GetGameInstance())
	{
		APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();

		if (PC)
		{
			PC->bShowMouseCursor = true;

			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;

			PC->bEnableTouchEvents = true;
			PC->bEnableTouchOverEvents = true;

			//PC->inpu = true;
		}
	}

}

void UAStarGISubsystem::OnTraceBaseClicked(UPrimitiveComponent * TouchedComponent, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("Trace Collision Actor Clicked!"));

	if (GetGameInstance() && bDrawDebug)
	{
		APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();

		if (PC && ButtonPressed.IsMouseButton())
		{
			FHitResult HitResult;

			PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);

			if (HitResult.bBlockingHit)
			{
				FVector Extents = FVector(CellSize / 2, CellSize / 2, CellSize / 2);

				FIntPoint Address = GetCellAddressFromWorldLocation(HitResult.Location);

				if (DoesCellExist(Address))
				{
					DrawDebugBox(GetWorld(), GetCellInfoByAddress(Address).Location, Extents, FColor::Emerald, false, 6.0f);
				}

			}

		}
	}
}

/*
	Assume the index using the address given.
*/
int32 UAStarGISubsystem::GetIndexFromAddress(FIntPoint InAddress) const
{
	return (GridExtents.X * InAddress.Y) + InAddress.X;
}



/*
	Calculate the index of where the InAddress cell should exist then compare it's address.
	If the addresses match the cell does exist.
*/
bool UAStarGISubsystem::DoesCellExist(FIntPoint InAddress) const
{
	int32 Index = (GridExtents.X * InAddress.Y) + InAddress.X;

	/*if (CellArray.IsValidIndex(Index) && CellArray[Index].Address != InAddress)
	{
		UE_LOG(LogTemp, Warning, TEXT("ArrayAddress: %s | Input Address: %s"), *CellArray[Index].Address.ToString(), *InAddress.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Failed. %s | %s | %s vs %s"), (CellArray.IsValidIndex(Index) ? TEXT("Valid Index") : TEXT("invalid Index")), (CellArray[Index].Address != InAddress ? TEXT("Addresses Dont Match") : TEXT("Addresses Match")), *CellArray[Index].Address.ToString(), *InAddress.ToString());
	}*/

	return(CellArray.IsValidIndex(Index) && CellArray[Index].Address == InAddress);
}



/*
	Check if a cell is walkable or not.
	Should ALWAYS CellExist() before calling this as this does compare addresses or if array index is valid
*/
bool UAStarGISubsystem::IsCellBlocked(FIntPoint InAddress) const
{
	int32 Index = (GridExtents.X * InAddress.Y) + InAddress.X;

	return(CellArray[Index].bBlocked);
}



/*
	Set a cell to be walkable or not.
	Safe to use without CellExist() as it checks first.
*/
void UAStarGISubsystem::SetCellBlocked(FIntPoint InAddress, bool bBlocked)
{
	if (DoesCellExist(InAddress))
	{
		int32 Index = (GridExtents.X * InAddress.Y) + InAddress.X;

		CellArray[Index].bBlocked = bBlocked;
	}

}



//As long as we add cells to the array on order (0-0, 1-0, 2-0, etc) we can use (GridExtent.X*CellAddress.Y)+CellAddress.X to calulate the cell's index in CellArray.
FGridCellInfo UAStarGISubsystem::GetCellInfoByAddress(FIntPoint Address) const
{
	if (DoesCellExist(Address))
	{
		int32 Index = (GridExtents.X * Address.Y) + Address.X;

		return CellArray[Index];
	}

	return FGridCellInfo();
}



FIntPoint UAStarGISubsystem::GetCellAddressFromWorldLocation(FVector WorldLocation)
{
	if (WorldLocation.X < 0 || WorldLocation.Y < 0 || WorldLocation.X > CellSize*GridExtents.X || WorldLocation.Y > CellSize*GridExtents.Y)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Failed at first range check"));
		return FIntPoint(-1, -1);
	}


	FIntPoint Address;
	Address.X = FMath::TruncToFloat(WorldLocation.X / CellSize);
	Address.Y = FMath::TruncToFloat(WorldLocation.Y / CellSize);

	//check for out of range
	if (Address.X < 0 || Address.Y < 0 || Address.X > (CellSize*GridExtents.X) - 1.0f || Address.Y >(CellSize*GridExtents.Y) - 1.0f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Failed at Address range check: %s"), *Address.ToString());
		return FIntPoint(-1, -1);
	}


	//check if cell exist
	if (DoesCellExist(Address))
	{
		return Address;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Failed at CellExist check"));
	return FIntPoint(-1, -1);
}



TArray<FGridCellInfo> UAStarGISubsystem::GetCellNeighborsFromAddress(FIntPoint Address, bool bCorners) const
{
	TArray<FGridCellInfo> Neighbours;
	FIntPoint CellAddress;
	FIntPoint ClampedCellAddress;

	if (!DoesCellExist(Address))
		return  Neighbours;

	int32 X = Address.X;
	int32 Y = Address.Y;


	/*
	*	we clamp the address to compare it to the original, if they dont match the original was
	*	out of bounds. Stops the adding of dup cells due to clamping.

	snippet below

	CellAddress = FString::FromInt(X - 1) + "," + FString::FromInt(Y);
	ClampedCellAddress = FString::FromInt(FMath::Clamp(X - 1, 0, FMath::TruncToInt(GridExtents.X - 1))) + "," + FString::FromInt(Y);
	if (CellAddress != "" && CellExist(CellAddress) && GetCellInfoByAddress(CellAddress).Address != "" && CellAddress == ClampedCellAddress)
	Neighbours.Add(GetCellInfoByAddress(CellAddress));

	*/

	//X +
	CellAddress = FIntPoint(X + 1, Y);
	ClampedCellAddress = FIntPoint(FMath::Clamp(X + 1, 0, GridExtents.X - 1), Y);
	if (DoesCellExist(CellAddress) && CellAddress == ClampedCellAddress)
		Neighbours.Add(GetCellInfoByAddress(CellAddress));

	//X -
	CellAddress = FIntPoint(X - 1, Y);
	ClampedCellAddress = FIntPoint(FMath::Clamp(X - 1, 0, GridExtents.X - 1), Y);
	if (DoesCellExist(CellAddress) && CellAddress == ClampedCellAddress)
		Neighbours.Add(GetCellInfoByAddress(CellAddress));

	//Y +
	CellAddress = FIntPoint(X, Y + 1);
	if (DoesCellExist(CellAddress))
		Neighbours.Add(GetCellInfoByAddress(CellAddress));

	//Y -
	CellAddress = FIntPoint(X, Y - 1);
	if (DoesCellExist(CellAddress))
		Neighbours.Add(GetCellInfoByAddress(CellAddress));


	if (bCorners)
	{
		//X-, Y+
		CellAddress = FIntPoint(X - 1, Y + 1);
		ClampedCellAddress = FIntPoint(FMath::Clamp(X - 1, 0, GridExtents.X - 1), Y + 1);
		if (DoesCellExist(CellAddress) && CellAddress == ClampedCellAddress)
			Neighbours.Add(GetCellInfoByAddress(CellAddress));

		//X+, Y-
		CellAddress = FIntPoint(X + 1, Y - 1);
		ClampedCellAddress = FIntPoint(FMath::Clamp(X + 1, 0, GridExtents.X - 1), Y - 1);
		if (DoesCellExist(CellAddress) && CellAddress == ClampedCellAddress)
			Neighbours.Add(GetCellInfoByAddress(CellAddress));

		//X-, Y-
		CellAddress = FIntPoint(X - 1, Y - 1);
		ClampedCellAddress = FIntPoint(FMath::Clamp(X - 1, 0, GridExtents.X - 1), Y - 1);
		if (DoesCellExist(CellAddress) && CellAddress == ClampedCellAddress)
			Neighbours.Add(GetCellInfoByAddress(CellAddress));

		//X+, Y+
		CellAddress = FIntPoint(X + 1, Y + 1);
		ClampedCellAddress = FIntPoint(FMath::Clamp(X + 1, 0, GridExtents.X - 1), Y + 1);
		if (DoesCellExist(CellAddress) && CellAddress == ClampedCellAddress)
			Neighbours.Add(GetCellInfoByAddress(CellAddress));
	}

	return Neighbours;
}



TArray<FGridCellInfo> UAStarGISubsystem::GetWalkableCellNeighborsFromAddress(FIntPoint Address, bool bCorners) const
{
	TArray<FGridCellInfo> Neighbors;

	for (FGridCellInfo cell : GetCellNeighborsFromAddress(Address, bCorners))
	{
		if (!cell.bBlocked)
		{
			Neighbors.Add(cell);
		}
	}

	return Neighbors;
}

TArray<FGridCellInfo> UAStarGISubsystem::GetCircleFillAroundCell(FIntPoint Address, float Radius) const
{
	TArray<FGridCellInfo> CircleSelectedCells;

	if (!DoesCellExist(Address))
		return  CircleSelectedCells;

	const int NumOfCells = ((Radius/CellSize)*2)+1;
	const int StartX = FMath::Clamp(Address.X - (NumOfCells-1)/2, 0, GridExtents.X-1);
	const int StartY = FMath::Clamp(Address.Y - (NumOfCells-1)/2, 0, GridExtents.Y-1);
	int X = StartX;
	int Y = StartY;

	UE_LOG(LogTemp, Warning, TEXT("Address: %s | NumOfCells: %d | StartX: %d | StartY: %d"),*Address.ToString(), NumOfCells, StartX, StartY);
	
	while(Y < StartY+NumOfCells)
	{
		//UE_LOG(LogTemp, Warning, TEXT("X: %d | Y: %d"), X, Y);

		if (DoesCellExist(FIntPoint(X, Y)))
		{
			const float DistanceToCenter = (GetCellInfoByAddress(FIntPoint(X, Y)).Location - GetCellInfoByAddress(Address).Location).Size();

			if(DistanceToCenter <= Radius)
				CircleSelectedCells.Add(GetCellInfoByAddress(FIntPoint(X, Y)));
		}


		if(X == StartX+NumOfCells)
		{
			X = StartX;
			++Y;
		}
		else
		{
			++X;
		}
	}

	return CircleSelectedCells;
}


int32 UAStarGISubsystem::GetHeuristic(FIntPoint GoalAddress, FIntPoint StartAddress)
{
	if (CellArray.IsValidIndex(0) && DoesCellExist(StartAddress) && DoesCellExist(GoalAddress))
	{
		//float H = (GridManager->GetCellInfoByAddress(StartAddress).Location - GridManager->GetCellInfoByAddress(GoalAddress).Location).Size();
		int32 GX = GetCellInfoByAddress(GoalAddress).Address.X;
		int32 GY = GetCellInfoByAddress(GoalAddress).Address.Y;

		int32 TX = GetCellInfoByAddress(StartAddress).Address.X;
		int32 TY = GetCellInfoByAddress(StartAddress).Address.Y;

		return FMath::Abs(GX - TX) + FMath::Abs(GY - TY);
		//return H;
	}

	return 0.0f;
}



// A* (star) Pathfinding
//https://medium.com/@nicholas.w.swift/easy-a-star-pathfinding-7e6689c7f7b2
FAStarSearchResults UAStarGISubsystem::AStarSearchToGoal(FIntPoint Start, FIntPoint Goal, bool bCorners)
{

	if (!DoesCellExist(Start) || !DoesCellExist(Goal))
		return FAStarSearchResults();


	// Initialize both open and closed list
	TArray<FAStarCellInfo> OpenList;
	TArray<FAStarCellInfo> ClosedList;
	TMap<FIntPoint, FIntPoint> CameFrom;

	// Add the start node
	FAStarCellInfo StartNode = FAStarCellInfo(Start, FIntPoint(-1, -1));
	StartNode.G = 0;
	StartNode.F = StartNode.G + FMath::Max(FMath::Abs(Start.X - Goal.X), FMath::Abs(Start.Y - Goal.Y));

	OpenList.Add(StartNode);

	// Loop until you find the end
	while (OpenList.Num() > 0)
	{
		// Get the current node
		//let the currentNode equal the node with the least f value
		FAStarCellInfo CurrentNode = OpenList[0];

		for (FAStarCellInfo node : OpenList)
		{
			if (node.F < CurrentNode.F)
			{
				CurrentNode = node;
			}
		}


		// Found the goal
		//if currentNode is the goal
		if (CurrentNode.Address == Goal)
		{
			FAStarSearchResults results;
			results.FoundGoal = true;

			FAStarCellInfo current = CurrentNode;

			while (current.CameFrom != FIntPoint(-1, -1))
			{
				results.Path.Add(current.Address);
				current = *ClosedList.FindByPredicate([&](FAStarCellInfo& InItem)
					{
						return InItem.Address == current.CameFrom;
					});
			}

			Algo::Reverse(results.Path);
			return results;
		}


		//remove the currentNode from the openList
		for (int i = 0; i < OpenList.Num(); i++)
		{
			if (OpenList[i].Address == CurrentNode.Address)
			{
				OpenList.RemoveAt(i);
			}

		}

		//add the currentNode to the closedList
		ClosedList.Add(CurrentNode);


		

		// Generate children
		//let the children of the currentNode equal the adjacent nodes
		TArray<FGridCellInfo> ChildrenNodes = GetWalkableCellNeighborsFromAddress(CurrentNode.Address, bCorners);

		for (FGridCellInfo cell : ChildrenNodes)
		{
			// Child is NOT on the closedList
			FAStarCellInfo ChildCell = FAStarCellInfo(cell.Address, CurrentNode.Address);

			if (!ClosedList.Contains(ChildCell) && !IsCellBlocked(ChildCell.Address))
			{
				// Create the f, g, and h values
				ChildCell.G = CurrentNode.G + 1;
				ChildCell.H = FMath::Max(FMath::Abs(ChildCell.Address.X - Goal.X), FMath::Abs(ChildCell.Address.Y - Goal.Y));
				ChildCell.F = ChildCell.G + ChildCell.H;

				bool AddToOpen = true;
				//Child is NOT already in the open list or g
				for (FAStarCellInfo OpenNode : OpenList)
				{
					if (ChildCell == OpenNode && ChildCell.G >= OpenNode.G)
					{
						AddToOpen = false;
						break;
					}
				}

				if (AddToOpen)
				{
					OpenList.Add(ChildCell);
				}
			}
		}
	}


	return FAStarSearchResults();
}

void UAStarGISubsystem::AStarSearchToGoalAsync(FIntPoint Start, FIntPoint Goal, bool bCorners)
{

	(new FAutoDeleteAsyncTask<FStarSearchToGoalTask>(this, OnAStarSearch, Start, Goal, bCorners))->StartBackgroundTask();
    
}


void UAStarGISubsystem::OnGridCreated_Internal(const FIntPoint& gridExtents, const TArray<FGridCellInfo>& OutCellArray, const float OutCellSize)
{
	//CellArray = OutCellArray;
	//GridExtents = gridExtents;
	//CellSize = OutCellSize;
	//SetupTraceCollisionActor();

	//OnWorldGridCreated.Broadcast(GridExtents, CellArray, CellSize);


	//AsyncTask(ENamedThreads::GameThread, [this, gridExtents, OutCellArray, OutCellSize]()
	//{
		//code to execute on game thread here
		//CellArray = OutCellArray;
		//GridExtents = gridExtents;

		//SetupTraceCollisionActor();
		//(new FAutoDeleteAsyncTask<FSetUpGridTask>(this, OutCellArray, GridExtents, OutCellSize))->StartSynchronousTask();
	//});

	//OutCellArray = CreateEmptyGridFloor(gridExtents.X, gridExtents.Y);
}

void UAStarGISubsystem::AStarSearchToGoal_Internal(const FAStarSearchResults SearchResults)
{
	UE_LOG(LogTemp, Warning, TEXT("Finished Async search"));
}

void UAStarGISubsystem::OnGridCreated(const FIntPoint & gridExtents, const TArray<FGridCellInfo>& OutCellArray, const float OutCellSize)
{
	CellArray = OutCellArray;
	GridExtents = gridExtents;
	CellSize = OutCellSize;
	SetupTraceCollisionActor();
}



void UAStarGISubsystem::CreateWorldGrid(FIntPoint WorldExtent, float WorldCellSize)
{
	if (WorldExtent.X < 5)
		WorldExtent.X = DefaultGridExtents.X;

	if (WorldExtent.Y < 5)
		WorldExtent.Y = DefaultGridExtents.Y;

	if (WorldCellSize < 25)
		WorldCellSize = DefaultCellSize;

	//AsyncTask(ENamedThreads::GameThread, [this, WorldExtent, WorldCellSize]()
		//{
	this->GridExtents = WorldExtent;
	this->CellSize = WorldCellSize;
	(new FAutoDeleteAsyncTask<FCreateWorldGridTask>(this, OnWorldGridCreated_Internal, WorldExtent, WorldCellSize))->StartBackgroundTask();
		//});
}



FString UAStarGISubsystem::GetDebugMessage()
{
	return "This be the subsysten";
}