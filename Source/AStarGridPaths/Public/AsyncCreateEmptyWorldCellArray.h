// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AStarSSTypes.h"
#include "AsyncCreateEmptyWorldCellArray.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAsyncCreateEmptyWorldOutputPin, FIntPoint, OutExtents, float, OutCellSize, const TArray<FGridCellInfo>&, OutCellArray);
/**
 * 
 */
UCLASS()
class ASTARGRIDPATHS_API UAsyncCreateEmptyWorldCellArray : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FAsyncCreateEmptyWorldOutputPin AfterCreateEmptyWorld;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Flow Control")
	static UAsyncCreateEmptyWorldCellArray* AsyncCreateEmptyWorld(const UObject* WorldContextObject, const FIntPoint Extents, const float CellSize);

	// UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	
	//~UBlueprintAsyncActionBase interface
	private:
	UFUNCTION()
	void ExecuteAfterCreateEmptyWorld();

private:
	const UObject* WorldContextObject;
	FIntPoint InExtents;
	float InCellSize;
};
