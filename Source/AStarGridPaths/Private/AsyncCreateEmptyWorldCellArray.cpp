// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncCreateEmptyWorldCellArray.h"

#include "AStarGISubsystem.h"

UAsyncCreateEmptyWorldCellArray::UAsyncCreateEmptyWorldCellArray(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), WorldContextObject(nullptr), InExtents(FIntPoint(10,10)), InCellSize(100.0f)
{
	
}

UAsyncCreateEmptyWorldCellArray* UAsyncCreateEmptyWorldCellArray::AsyncCreateEmptyWorld(const UObject* WorldContextObject, const FIntPoint Extents, const float CellSize)
{
	UAsyncCreateEmptyWorldCellArray* BlueprintNode = NewObject<UAsyncCreateEmptyWorldCellArray>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->InExtents = Extents;
	BlueprintNode->InCellSize = CellSize;
	return BlueprintNode;
}

void UAsyncCreateEmptyWorldCellArray::Activate()
{
	// Any safety checks should be performed here. Check here validity of all your pointers etc.
	// You can log any errors using FFrame::KismetExecutionMessage, like that:
	// FFrame::KismetExecutionMessage(TEXT("Valid Player Controller reference is needed for ... to start!"), ELogVerbosity::Error);
	// return;
	UAStarGISubsystem* AStarSubSys = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UAStarGISubsystem>();
	//UE_LOG(LogTemp, Warning, TEXT("%s!"), *AStarSubSys->GetDebugMessage());
	if(AStarSubSys)
	{
		TArray<FGridCellInfo> GridArray;
		
		//If no values are set, get them from our class defaults.
		if (InExtents.X < 2 || InExtents.Y < 2)
		{
			InExtents = AStarSubSys->DefaultGridExtents;
		}
		if (InCellSize <= 10.0f)
		{
			InCellSize = AStarSubSys->DefaultCellSize;
		}

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
			//GridArray.Add(FGridCellInfo(FIntPoint(X, Y), FVector(X*InCellSize + (InCellSize / 2), Y*InCellSize + (InCellSize / 2), 0), Index));

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
		//return
		AfterCreateEmptyWorld.Broadcast(InExtents, InCellSize, GridArray);
	}
	
	
	//WorldContextObject->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UAsyncCreateEmptyWorldCellArray::ExecuteAfterCreateEmptyWorld);
}

void UAsyncCreateEmptyWorldCellArray::ExecuteAfterCreateEmptyWorld()
{
	//
	
}