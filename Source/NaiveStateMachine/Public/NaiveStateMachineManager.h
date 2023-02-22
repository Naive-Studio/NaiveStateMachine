// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveStateMachineTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "NaiveStateMachineTypes.h"
#include "NaiveStateMachineManager.generated.h"

class UNaiveSMNodeBase;
class UNaiveStateMachine;
class UNaiveStateNode;
class UNaiveTransitionNode;

/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveStateMachineManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UNaiveStateMachineManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	FNaiveStateMachineHandle StartStateMachine(const FNaiveRunStateMachineRequest& InRequest);
	void StopStateMachine(const FNaiveStateMachineHandle& InHandle);

	FName GetActiveStateName(const FNaiveStateMachineHandle& InHandle);

	// FTickableGameObject begin 
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	// FTickableGameObject end

	void GoToState(FNaiveStateMachineContext& StateMachineContext, UNaiveStateMachine* InStateMachine , const FName& NewState);
	UNaiveStateNode* GetActiveState(const FNaiveStateMachineHandle& InHandle);

	void PostEventToStateMachine(const FNaiveStateMachineHandle& InHandle, const FName& InEventName);

protected:
	void InnerGoToState(FNaiveStateMachineContext& StatMachineContext,
						UNaiveStateMachine* InStateMachine,
						FName NewState, int32& InMemorySize);

	static FName BuildStateName(const TArray<FNaiveStateContext>& StateStack);

protected:
	UPROPERTY(transient)
	TMap<FNaiveStateMachineHandle, FNaiveStateMachineContext> RunningStateMachines;
	
	TMap <FNaiveStateMachineHandle, FNaiveEventQueue> ReceivedEventMap;
};
