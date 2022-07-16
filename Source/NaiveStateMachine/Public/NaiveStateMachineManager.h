// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveStateMachineTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "NaiveStateMachineTypes.h"
#include "NaiveStateMachineManager.generated.h"

class UNaiveStateMachineObjectBase;
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

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;
	
	FNaiveStateMachineHandle StartStateMachine(const FNaiveRunStateMachineRequest& InRequest);
	void StopStateMachine(const FNaiveStateMachineHandle& InHandle);

	FName GetActiveStateName(const FNaiveStateMachineHandle& InHandle);

	// FTickableGameObject begin 
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	// FTickableGameObject end

	void SwitchState(FNaiveStateMachineInstance& StateMachineContext, UNaiveStateMachine* InStateMachine , const FName& NewState);
	UNaiveStateNode* GetActiveState(const FNaiveStateMachineHandle& InHandle);

	void SendEventToTransition(const FNaiveStateMachineHandle& InHandle, const FName& InEventName);

protected:
	void LoadStateMachine(FNaiveStateMachineInstance& InstanceContext, const FNaiveRunStateMachineRequest& InRequest);

	void InnerEnterState(FNaiveStateMachineInstance& StatMachineContext,
						UNaiveStateMachine* InStateMachine,
						FName NewState, int32& InMemorySize);

	static FName MakeLayeredStateName(const TArray<FNaiveStateInstance>& StateStack);

protected:
	UPROPERTY(transient)
	TMap<FNaiveStateMachineHandle, FNaiveStateMachineInstance> RunningStateMachines;
	
	TMap <FNaiveStateMachineHandle, FNaiveEventQueue> PendingForwardEvents;
};
