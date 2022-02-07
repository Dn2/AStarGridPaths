// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "GridMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineTickDel, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimelineStartDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimelineEndDel);


UCLASS( ClassGroup=(Grid), meta=(BlueprintSpawnableComponent), Blueprintable)
class ASTARGRIDPATHS_API UGridMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	UTimelineComponent* MoveLerpTimeline;

	UPROPERTY()
	UCurveFloat* fCurve;

	UPROPERTY()
	FOnTimelineFloat InterpMoveFunction {};

	UPROPERTY()
	FOnTimelineEvent InterpFinishMoveFunction {};

	UFUNCTION(BlueprintCallable, Category = Grid)
	void OnTimelineTick(float val);

	UFUNCTION(BlueprintCallable, Category = Grid)
	void OnTimelineBegin();
	
	UFUNCTION(BlueprintCallable, Category = Grid)
	void OnTimelineEnd();

	UFUNCTION(BlueprintCallable, Category = Grid)
	void StartMove();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Grid)
	float GameSpeed;

	UPROPERTY(BlueprintReadWrite, Category = Grid)
	float CurrentTurn;

	//Lets the component know that we need to stop after the last move to a point is complete.
	bool bPendingMovementInterrupt;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Grid")
	FOnTimelineTickDel OnTimelineTickDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Grid")
	FOnTimelineStartDel OnTimelineStartDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Grid")
	FOnTimelineEndDel OnTimelineEndDelegate;

	UPROPERTY(BlueprintReadWrite, Category = Grid)
	FIntPoint Address;

	FIntPoint FromAddress;
	FIntPoint ToAddress;

	UPROPERTY(BlueprintReadWrite, Category = Grid)
	TArray<FIntPoint> TargetPath;

	UFUNCTION(BlueprintCallable, Category = Grid)
	virtual void UpdateGridLocationData(FIntPoint NewAddress);

	//FOnTimelineStart
	//FOnTimelineTick
	//FOnTimelineEnd
};
