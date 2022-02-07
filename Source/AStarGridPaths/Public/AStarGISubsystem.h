// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"
#include "Async/Async.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TraceCollisionActor.h"
#include "AStarSSTypes.h"
#include "AStarGISubsystem.generated.h"

//DECLARE_DELEGATE_RetVal_<3>Params(FAStarSearchResults, FAStarSearchDel, FIntPoint, FIntPoint, bool);
//DECLARE_DELEGATE_RetVal(FAStarSearchResults, FAStarSearchDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAStarSearchDel, FAStarSearchResults, SearchResults);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAStarSearchToGoal_Internal, const FIntPoint, Start, const FIntPoint, Goal, const bool, bCorners);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWorldGridCreated_Internal, const FIntPoint&, gridExtents, const TArray<FGridCellInfo>&, OutCellArray, const float, OutCellSize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWorldGridCreated, const FIntPoint&, gridExtents, const TArray<FGridCellInfo>&, OutCellArray, const float, OutCellSize);

/**
 * 
 */
UCLASS()
class ASTARGRIDPATHS_API UAStarGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grid)
	EAStarState ASGIState;


	/**
	 *	Functions
	 */

		/* Grid */

	//Returns an array of cells, XExtent * YExtent in size.	This should maybe be private only used by other funtions here
	UFUNCTION(BlueprintCallable, Category = Grid)
	TArray<FGridCellInfo> CreateEmptyGridFloor(int32 XExtent, int32 YExtent, float Size) const;

	//Creates an empty grid "world" with only floor tiles. This should be the step before generating a level with different types of tiles. This can be public, but destroys the current world
	UFUNCTION(BlueprintCallable, Category = Grid)
    void CreateEmptyWorldArray(FIntPoint Extents, float Size);

	//Same as above but async
	UFUNCTION(BlueprintCallable, Category = Grid)
	void AsyncCreateEmptyWorldArray(FIntPoint Extents, float Size);

	UFUNCTION(BlueprintCallable, Category = Grid)
	void SetupTraceCollisionActor();
	
	UFUNCTION(BlueprintCallable, Category = Grid)
	void AutoSetupControllerEvents();

	UFUNCTION()
	void OnTraceBaseClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	//Convert Address to Index
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Grid)
	int32 GetIndexFromAddress(FIntPoint InAddress) const;


	//Cheap. Assumes cells index in array.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Grid)
	bool DoesCellExist(FIntPoint InAddress) const;


	//Cheap. Assumes cells index in array.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Grid)
	bool IsCellBlocked(FIntPoint InAddress) const;


	UFUNCTION(BlueprintCallable, Category = Grid)
	void SetCellBlocked(FIntPoint InAddress, bool bBlocked);


	UFUNCTION(BlueprintCallable, Category = Grid)
	FGridCellInfo GetCellInfoByAddress(FIntPoint Address) const;


	/*
		Get cell coordinates by world location.
		Instead of spawning actors or Box colliders for touch/mouseover events,
		we can just spawn a single large collision in the size of our grid for
		location info from a trace. Then use that location to get the cell address of the location.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Grid)
	FIntPoint GetCellAddressFromWorldLocation(FVector WorldLocation);


	// returns all neighbor cells, including corners if bCorners is true
	UFUNCTION(BlueprintCallable, Category = Grid)
	TArray<FGridCellInfo> GetCellNeighborsFromAddress(FIntPoint Address, bool bCorners = true) const;


	// returns all neighbor cells, including corners if bCorners is true
	UFUNCTION(BlueprintCallable, Category = Grid)
	TArray<FGridCellInfo> GetWalkableCellNeighborsFromAddress(FIntPoint Address, bool bCorners = true) const;

	// returns all neighbor cells, including corners if bCorners is true
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = Grid)
	TArray<FGridCellInfo> GetCircleFillAroundCell(FIntPoint Address, float Radius) const;
	
		/* AStar */

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = AStar)
	int32 GetHeuristic(FIntPoint GoalAddress, FIntPoint StartAddress);


	/* 
	*	A* (star) Pathfinding
	*	https://medium.com/@nicholas.w.swift/easy-a-star-pathfinding-7e6689c7f7b2
	*/
	UFUNCTION(BlueprintCallable, Category = AStar)
	FAStarSearchResults AStarSearchToGoal(FIntPoint Start, FIntPoint Goal, bool bCorners);

	UFUNCTION(BlueprintCallable, Category = AStar)
	void AStarSearchToGoalAsync(FIntPoint Start, FIntPoint Goal, bool bCorners);

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "AStar")
	FAStarSearchDel OnAStarSearch;

	UPROPERTY(/*BlueprintCallable, BlueprintAssignable, Category = "AStar"*/)
	FOnWorldGridCreated_Internal OnWorldGridCreated_Internal;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "AStar")
	FOnWorldGridCreated OnWorldGridCreated;

	UFUNCTION()
	void OnGridCreated_Internal(const FIntPoint& gridExtents , const TArray<FGridCellInfo>& OutCellArray,const float OutCellSize);

	//AStarSearchToGoal_Internal
	UFUNCTION()
	void AStarSearchToGoal_Internal(const FAStarSearchResults SearchResults);

	
	UFUNCTION()
	void OnGridCreated(const FIntPoint& gridExtents=FIntPoint(-1,-1) , const TArray<FGridCellInfo>& OutCellArray=TArray<FGridCellInfo>(),const float OutCellSize=100.0f);


	UFUNCTION(BlueprintCallable, Category = Grid)
	void CreateWorldGrid(FIntPoint WorldExtent, float WorldCellSize);
	/**
	 *	Variables
	 */

		/* Grid */

	//
	UPROPERTY(BlueprintReadWrite, Category = Grid)
	TArray<FGridCellInfo> CellArray;

	UPROPERTY(BlueprintReadWrite, Category = Grid)
	ATraceCollisionActor* TraceCollisionBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grid)
	FIntPoint GridExtents;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grid)
	FIntPoint DefaultGridExtents = FIntPoint(25, 25);


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grid)
	float CellSize;

	float DefaultCellSize = 100;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grid)
	bool bDrawDebug;



public:
	UFUNCTION(BlueprintCallable)
	FString GetDebugMessage();
};



/*
	Create our default grid and fire off OnWorldGridCreated event
*/
class FCreateWorldGridTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FCreateWorldGridTask>;

public:
	FCreateWorldGridTask(UAStarGISubsystem* Subsystem, FOnWorldGridCreated_Internal& Del, FIntPoint gridSize, float cellSize=100) :
		AStarSubsystem(Subsystem),
		Delegate(Del),
		GridSize(gridSize),
		GridCellSize(cellSize)
	{}

protected:
	UAStarGISubsystem* AStarSubsystem;
	FOnWorldGridCreated_Internal Delegate;
	FIntPoint GridSize;
	float GridCellSize;


	void DoWork()
	{
		// Schedule a task on any thread with a high or normal priority
		AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [this] ()
		{
			// Do some stuff here... 
			if(AStarSubsystem)
			{
				// Once the task is done schedule a task back to the gamethread.
				// This is optional depending on the situation, you don't have to go back to the mainthread.
				TArray<FGridCellInfo> GridArray = AStarSubsystem->CreateEmptyGridFloor(GridSize.X, GridSize.Y, GridCellSize);

				
				AsyncTask(ENamedThreads::GameThread, [this, GridArray] ()
				{
					AStarSubsystem->CellArray = GridArray;
					AStarSubsystem->GridExtents = GridSize;
					AStarSubsystem->CellSize = GridCellSize;
					AStarSubsystem->SetupTraceCollisionActor();
					if(AStarSubsystem->OnWorldGridCreated.IsBound())
					{
						AStarSubsystem->OnWorldGridCreated.Broadcast(GridSize, GridArray, GridCellSize);
					}
					//UE_LOG(LogTemp, Warning, TEXT("Ran| Extends: %d, %d"), GridSize.X, GridSize.Y);
					//AStarSubsystem->OnWorldGridCreated_Internal.Broadcast(GridSize, GridArray, GridCellSize);
				});
			}
		});
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FCreateWorldGridTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};


/*
	Create our default grid and fire off OnWorldGridCreated event
*/
class FSetUpGridTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FSetUpGridTask>;

public:
	FSetUpGridTask(UAStarGISubsystem* Subsystem, TArray<FGridCellInfo> InCellArray,FIntPoint gridSize, float cellSize = 100):
		AStarSubsystem(Subsystem),
		NewCellArray(InCellArray),
		GridSize(gridSize),
		GridCellSize(cellSize)
	{}

protected:
	UAStarGISubsystem* AStarSubsystem;
	TArray<FGridCellInfo> NewCellArray;
	FIntPoint GridSize;
	float GridCellSize;

	void DoWork()
	{
		if (AStarSubsystem)
		{
			AStarSubsystem->CellArray = NewCellArray;
			AStarSubsystem->GridExtents = GridSize;
			AStarSubsystem->CellSize = GridCellSize;

			UE_LOG(LogTemp, Warning, TEXT("Final Extents: %d, %d"), AStarSubsystem->CellArray.Num(), AStarSubsystem->GridExtents.Y );

			AStarSubsystem->SetupTraceCollisionActor();
			AStarSubsystem->OnWorldGridCreated.Broadcast(GridSize, NewCellArray, GridCellSize);
		}
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FSetUpGridTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};


class FStarSearchToGoalTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FStarSearchToGoalTask>;

public:
	FStarSearchToGoalTask(UAStarGISubsystem* Subsystem, FAStarSearchDel& Del,FIntPoint Start, FIntPoint Goal, bool bCorners) :
		AStarSubsystem(Subsystem),
		Delegate(Del),
		StartAddress(Start),
		GoalAddress(Goal),
		UseCorners(bCorners)
	{}

protected:
	UAStarGISubsystem* AStarSubsystem;
	FAStarSearchDel Delegate;
	FIntPoint StartAddress;
	FIntPoint GoalAddress;
	bool UseCorners;

	void DoWork()
	{
		// Schedule a task on any thread with a high or normal priority
		//AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [this] ()
		//{
			// Do some stuff here... 
			if(AStarSubsystem)
			{
				FAStarSearchResults Results = AStarSubsystem->AStarSearchToGoal(StartAddress, GoalAddress, UseCorners);
				FAStarSearchDel SearchDel = AStarSubsystem->OnAStarSearch;
				// Once the task is done schedule a task back to the gamethread.
				// This is optional depending on the situation, you don't have to go back to the mainthread.
				
				AsyncTask(ENamedThreads::GameThread, [SearchDel, Results] ()
				{
					SearchDel.Broadcast(Results);
					//Delegate.Broadcast(Results);
					// Return something or execute a delegate
				});
			}
		//});
	}

	// This next section of code needs to be here.  Not important as to why.

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FStarSearchToGoalTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};


class FCreateEmptyWorldArrayTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FCreateEmptyWorldArrayTask>;

public:
	FCreateEmptyWorldArrayTask(UAStarGISubsystem* Subsystem, FIntPoint Extents, float CellSize, FOnWorldGridCreated& Del ) :
		InAStarSubsystem(Subsystem),
		InExtents(Extents),
		InCellSize(CellSize),
		InDelegate(Del)
	{}

protected:
	UAStarGISubsystem* InAStarSubsystem;
	FIntPoint InExtents;
	float InCellSize;
	FOnWorldGridCreated& InDelegate;

	void DoWork()
	{
		// Schedule a task on any thread with a high or normal priority
		//AsyncTask(ENamedThreads::AnyHiPriThreadNormalTask, [this] ()
		//{
			// Do some stuff here... 
			if(InAStarSubsystem)
			{
				TArray<FGridCellInfo> GridArray;
				
				//If no values are set, get them from our class defaults.
				/*if (InExtents.X < 2 || InExtents.Y < 2)
				{
					InExtents = InAStarSubsystem->DefaultGridExtents;
				}
				if (InCellSize <= 10.0f)
				{
					InCellSize = InAStarSubsystem->DefaultCellSize;
				}*/

				/*
				* Zero'd out indexes for the cell creation loop.
				* X = colums, Y = rows, Index = the index of the cell in the cell array (CellArray).
				*/
				int32 X = 0;
				int32 Y = 0;
				int32 Index = 0;

				/*
				* Creating our grid...
				* Where Y = number of rows and X = number of columns.
				* So while Y is less than the number of rows we need (GridExtents.Y),
				* keep adding cells with the appropriate addresses.
				* Ends when we run out of rows.
				*/
				while (Y < InExtents.Y)
				{
					// Create a cell info with current address, location and index.
					GridArray.Add(FGridCellInfo(FIntPoint(X, Y), FVector(X*InCellSize + (InCellSize / 2), Y*InCellSize + (InCellSize / 2), 0), Index));

					//UE_LOG(LogTemp, Warning, TEXT("Creating: %s | Location: %s | Index: %d"), *FVector2D(X, Y).ToString(), *FVector(X*CellSize + (CellSize / 2), Y*CellSize + (CellSize / 2), 0).ToCompactString(), Index);

					// Increment the index for the next cell.
					++Index;

					// If we reach the end of GridExtent.X we reset X to 0 and increment Y,
					// moving us onto the next row of cells to start again
					if (X == InExtents.X - 1)
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

				FIntPoint NewExtents = InExtents;
				float NewCellSize = InCellSize;
				FOnWorldGridCreated NewDel = InDelegate;

				
				// Once the task is done schedule a task back to the gamethread.
				// This is optional depending on the situation, you don't have to go back to the mainthread.
				AsyncTask(ENamedThreads::GameThread, [NewDel, NewExtents, GridArray, NewCellSize] ()
				{
					NewDel.Broadcast(NewExtents, GridArray, NewCellSize);
					// Return something or execute a delegate
					//UE_LOG(LogTemp, Warning, TEXT("Extends: %d, %d"), InCellSize, InExtents.Y);
					//InAStarSubsystem->CellArray = NewCellArray;
					//InAStarSubsystem->SetupTraceCollisionActor();
				});
			}
		//});
	}

	// This next section of code needs to be here.  Not important as to why.

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}
};