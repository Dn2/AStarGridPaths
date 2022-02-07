#pragma once

#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
#include "AStarSSTypes.generated.h"

//state for running async to let the game know we are waiting for a build/result/etc
UENUM(BlueprintType)
enum class EAStarState : uint8
{
	ASS_Idle			UMETA(DisplayName = "Top"),
	ASS_SearchingPath	UMETA(DisplayName = "Top Right"),
	ASS_BuildingGrid	UMETA(DisplayName = "Top"),
	ASS_BuildingLevel	UMETA(DisplayName = "Top Right"),
};


/**
 * Not sure if used. Confirm before deleting.
 */
UENUM(BlueprintType)
enum class EAStarDirection : uint8
{
	ASD_Top				UMETA(DisplayName = "Top"),
	ASD_TopRight		UMETA(DisplayName = "Top Right"),
	ASD_Right			UMETA(DisplayName = "Right"),
	ASD_BottomRight		UMETA(DisplayName = "Bottom Right"),
	ASD_Bottom			UMETA(DisplayName = "Bottom"),
	ASD_BottomLeft		UMETA(DisplayName = "Bottom Left"),
	ASD_Left			UMETA(DisplayName = "Left"),
	ASD_TopLeft			UMETA(DisplayName = "Top Left"),
};


UENUM(BlueprintType)
enum class ECellType : uint8
{
	CT_Empty			UMETA(DisplayName = "Empty"),
	CT_Floor			UMETA(DisplayName = "Floor"),
	CT_Bridge			UMETA(DisplayName = "Bridge"),
	CT_Wall				UMETA(DisplayName = "Wall"),
	CT_StairsUp			UMETA(DisplayName = "Stairs Up"),
	CT_StairsDown		UMETA(DisplayName = "Stairs Down"),
	CT_Door				UMETA(DisplayName = "Door"),
	CT_DoorLocked		UMETA(DisplayName = "Locked Door"),
	CT_Hole				UMETA(DisplayName = "Hole"),
	CT_Water			UMETA(DisplayName = "Water"),
	CT_Toxic			UMETA(DisplayName = "Toxic Material")
};


/**
 *	Struct holding the results of algorithm.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "AStar Search Info"))
struct FAStarSearchResults
{
	GENERATED_USTRUCT_BODY()

public:

	/** Default constructor (no initialization). */
	FORCEINLINE FAStarSearchResults();
	FORCEINLINE FAStarSearchResults(TMap<FIntPoint, FIntPoint> InCameFrom, TMap<FIntPoint, int32> InCostSoFar);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Search Results")
	bool FoundGoal;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Search Results")
	bool FoundGoald;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Search Results")
	TMap<FIntPoint, FIntPoint> CameFrom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Search Results")
	TMap<FIntPoint, int32> CostSoFar;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Search Results")
	TArray<FIntPoint> Path;
};

FORCEINLINE FAStarSearchResults::FAStarSearchResults()
{}

FORCEINLINE FAStarSearchResults::FAStarSearchResults(TMap<FIntPoint, FIntPoint> InCameFrom, TMap<FIntPoint, int32> InCostSoFar)
	: CameFrom(InCameFrom), CostSoFar(InCostSoFar)
{}



/**
 *
 */
USTRUCT(BlueprintType, meta = (DisplayName = "AStar Search Info"))
struct FAStarSearchInfo
{
	GENERATED_USTRUCT_BODY()

public:
	/** Default constructor (no initialization). */
	FORCEINLINE FAStarSearchInfo();
	FORCEINLINE FAStarSearchInfo(FIntPoint InAddress, float InCost);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Search Info")
	FIntPoint Address;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Search Info")
	float Cost;
};

FORCEINLINE FAStarSearchInfo::FAStarSearchInfo()
{}

FORCEINLINE FAStarSearchInfo::FAStarSearchInfo(FIntPoint InAddress, float InCost)
	: Address(InAddress), Cost(InCost)
{}



/**
 *	Used for the A* algo
 */
USTRUCT(BlueprintType, meta = (DisplayName = "AStar Cell Info"))
struct FAStarCellInfo
{
	GENERATED_USTRUCT_BODY()

public:
	/** Default constructor (no initialization). */
	FORCEINLINE FAStarCellInfo();

	FORCEINLINE FAStarCellInfo(FIntPoint InAddress, FIntPoint InCameFrom);

	FORCEINLINE FAStarCellInfo(float InH, float InG, bool InbVisited, FIntPoint InCameFrom, FIntPoint InAddress);


	bool operator==(const FAStarCellInfo& OtherItem) const
	{
		return OtherItem.Address == Address;
	}

	//Heuristic
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	float H;


	//Distance Traveled
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	float G;

	//Distance Traveled
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	float F;

	//has cell already been visited
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	bool bVisited;


	//direction we came from
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	EAStarDirection ECameFrom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	FIntPoint Address;

	//direction we came from
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	FIntPoint CameFrom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	FVector Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	int32 Index;
};

FORCEINLINE FAStarCellInfo::FAStarCellInfo()
{}

FORCEINLINE FAStarCellInfo::FAStarCellInfo(FIntPoint InAddress, FIntPoint InCameFrom)
	: Address(InAddress), CameFrom(InCameFrom)
{}

FORCEINLINE FAStarCellInfo::FAStarCellInfo(float InH, float InG, bool InbVisited, FIntPoint InCameFrom, FIntPoint InAddress)
	: H(InH), G(InG), bVisited(InbVisited), Address(InAddress), CameFrom(InCameFrom)
{}



/**
 *	Used for grid array
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Grid Cell Info"))
struct FGridCellInfo
{
	GENERATED_USTRUCT_BODY()

public:
	/** Default constructor (no initialization). */
	FORCEINLINE FGridCellInfo();
	FORCEINLINE FGridCellInfo(FIntPoint InAddress, FVector InLocation, int32 InIndex);

	UPROPERTY(BlueprintReadWrite, Category = "Cell Info")
	FIntPoint Address = FIntPoint(-1, -1);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	FVector Location;

	//The index of the array this cell would be at. Set when generation arrays.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	int32 Index = 0;

	//The index of the array this cell would be at. Set when generation arrays.
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	int32 MeshInstanceIndex = 0;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	float Cost = 0;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	bool bBlocked = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cell Info")
	ECellType CellType = ECellType::CT_Floor;
};


FORCEINLINE FGridCellInfo::FGridCellInfo()
{}


FORCEINLINE FGridCellInfo::FGridCellInfo(FIntPoint InAddress, FVector InLocation, int32 InIndex)
	: Address(InAddress), Location(InLocation), Index(InIndex)
{}