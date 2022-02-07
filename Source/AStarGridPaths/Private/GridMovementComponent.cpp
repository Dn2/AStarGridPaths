// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMovementComponent.h"
#include "AStarGISubsystem.h"

// Sets default values for this component's properties
UGridMovementComponent::UGridMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	MoveLerpTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MoevLerpTLComp"));

	if (GameSpeed <= 0.0f)
		GameSpeed = 1;

}


// Called when the game starts
void UGridMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InterpMoveFunction.BindUFunction(this, FName{ TEXT("OnTimelineTick") });
	InterpFinishMoveFunction.BindUFunction(this, FName{ TEXT("OnTimelineEnd") });

	//Create curve based on GameSpeed for our TimeLine
	fCurve = NewObject<UCurveFloat>();
	fCurve->bIsEventCurve = true;
	fCurve->FloatCurve.AddKey(0.0f, 0.0f);
	fCurve->FloatCurve.AddKey(GameSpeed, 1.0f);

	if (MoveLerpTimeline && fCurve)
	{
		MoveLerpTimeline->AddInterpFloat(fCurve, InterpMoveFunction, FName{ TEXT("Floaty") });
		MoveLerpTimeline->SetTimelineFinishedFunc(InterpFinishMoveFunction);

		//UE_LOG(LogTemp, Warning, TEXT("TimeLine and Curve not null! new"));
	}

}


// Called every frame
void UGridMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGridMovementComponent::OnTimelineTick(float val)
{
	OnTimelineTickDelegate.Broadcast(val);

	AActor* OwningActor = GetOwner();
	if (OwningActor && !OwningActor->IsPendingKillPending() && TargetPath.IsValidIndex(0))
	{
		if (val == 0.0f)
		{
			FromAddress = Address;
			UpdateGridLocationData(TargetPath[0]);
			ToAddress = Address;

			//++CurrentTurn;
			//OnTimelineBegin();
			OnTimelineStartDelegate.Broadcast();
		}

		UAStarGISubsystem* MySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAStarGISubsystem>();

		if (MySubsystem)
		{
			FVector TargetLoc = MySubsystem->GetCellInfoByAddress(ToAddress).Location;
			OwningActor->SetActorLocation(FMath::LerpStable(MySubsystem->GetCellInfoByAddress(FromAddress).Location, TargetLoc, val));
		}
		//UE_LOG(LogTemp, Warning, TEXT("Ran: GM Tick = %f"), val);
	
	}
}

void UGridMovementComponent::OnTimelineBegin()
{
	//OnTimelineStartDelegate.Broadcast();
}

void UGridMovementComponent::OnTimelineEnd()
{
	OnTimelineEndDelegate.Broadcast();

	AActor* OwningActor = GetOwner();
	//stop if bPendingMovementInterrupt is true and return
	if (bPendingMovementInterrupt)
	{
		//PlayerActor->LastAddress = PlayerActor->Address;
		bPendingMovementInterrupt = false;

		//remove previous address from TargetPath
		if (OwningActor && TargetPath.Num() > 0)
		{
			TargetPath.RemoveAt(0);
			//PlayerActor->LastAddress = PlayerActor->Address;
		}

		return;
	}

	//Continue playing another movement loop if there is no pending interrupts TargetPath isn't empty
	if (OwningActor && TargetPath.Num() > 0)
	{
		TargetPath.RemoveAt(0);

		if (!bPendingMovementInterrupt && TargetPath.Num() > 0)
		{
			//PlayerActor->LastAddress = PlayerActor->Address;
			MoveLerpTimeline->PlayFromStart();
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("TimeLine Finished Function Called!"));
}

void UGridMovementComponent::StartMove()
{
	if (MoveLerpTimeline && fCurve && !MoveLerpTimeline->IsPlaying())
	{
		MoveLerpTimeline->PlayFromStart();
	}
}

void UGridMovementComponent::UpdateGridLocationData(FIntPoint NewAddress)
{
	Address = NewAddress;
}

