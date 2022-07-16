// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "NaiveStateMachineTypes.generated.h"

class UNaiveStateMachineObjectBase;
class UNaiveStateMachine;
class UNaiveStateNode;
class UNaiveTransitionNode;
class UClass;

NAIVESTATEMACHINE_API DECLARE_LOG_CATEGORY_EXTERN(LogSM, Display, All);


USTRUCT(BlueprintType)
struct NAIVESTATEMACHINE_API FNaiveStateMachineHandle
{
	GENERATED_USTRUCT_BODY()
public:
	FNaiveStateMachineHandle()
		:Handle(INDEX_NONE)
	{

	}

	FNaiveStateMachineHandle(int32 Index)
		:Handle(Index)
	{

	}

	bool IsValid() const
	{
		return Handle != INDEX_NONE;
	}

	void Invalidate()
	{
		Handle = INDEX_NONE;
	}

	static FNaiveStateMachineHandle GenerateHandle();

	bool operator==(const FNaiveStateMachineHandle& Other) const
	{
		return Handle == Other.Handle;
	}

	bool operator!=(const FNaiveStateMachineHandle& Other) const
	{
		return Handle != Other.Handle;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("%d"), Handle);
	}

	int32 ToInt() const
	{
		return Handle;
	}

	static FNaiveStateMachineHandle HANDLE_NONE;

private:

	friend uint32 GetTypeHash(const FNaiveStateMachineHandle& InHandle)
	{
		return GetTypeHash(InHandle.Handle);
	}

protected:
	UPROPERTY(Transient)
	int32 Handle = INDEX_NONE;

	static int32 NextHandle;
};

/* Old State Name, New State Name*/
DECLARE_DELEGATE_TwoParams(FNaiveStateChangeSignature, FName, FName);

/* Owner, Handle*/
DECLARE_DELEGATE_TwoParams(FNaiveStateMachineStopSignature, UObject* ,FNaiveStateMachineHandle);

USTRUCT(BlueprintType)
struct NAIVESTATEMACHINE_API FNaiveRunStateMachineRequest
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
	UNaiveStateMachine* Template = nullptr;

	UPROPERTY(EditAnywhere)
	UObject* Owner = nullptr;

	UPROPERTY(EditAnywhere)
	UWorld* World = nullptr;

	FNaiveStateChangeSignature Delegate;
	FNaiveStateMachineStopSignature OnStoppedDelegate;
};

#define NAIVE_INVALID_STATE NAME_None


USTRUCT()
struct FNaiveTransition
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UNaiveTransitionNode> TransitionTemplate = nullptr;

	UPROPERTY(EditAnywhere)
	FName NextSate = NAIVE_INVALID_STATE;

	UPROPERTY(EditAnywhere)
	bool bEventDriven = false;

	UPROPERTY(EditAnywhere)
	TArray<FName> ObservingEvents;
};


USTRUCT()
struct FNaiveState
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UNaiveStateNode> StateTemplate = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<FNaiveTransition> Transitions;

	UPROPERTY(EditAnywhere)
	class UNaiveStateMachine* SubStateMachineAsset = nullptr;
};


USTRUCT()
struct FNaiveInstance
{
	GENERATED_USTRUCT_BODY()
public:

	bool bInstanced = false;

	int32 InstanceMemoryOffset = -1;

	uint16 GetInstanceMemoryOffset() const { return InstanceMemoryOffset;}
};


USTRUCT()
struct FNaiveTransitionInstance : public FNaiveInstance
{
	GENERATED_USTRUCT_BODY()
public:
	FName NextSate = NAIVE_INVALID_STATE;
	
	TArray<FName> ObservingEvents; 

	UPROPERTY(Transient)
	UNaiveTransitionNode* TransitionObject = nullptr;
};


USTRUCT()
struct FNaiveStateInstance : public FNaiveInstance
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FName StateName = NAME_None;
	
	UPROPERTY(Transient)
	UNaiveStateNode* StateNodeInstance = nullptr;
	
	UPROPERTY()
	TArray<FNaiveTransitionInstance> ActiveTransitions;
	
	UPROPERTY(Transient)
	class UNaiveStateMachine* OwnerStateMachine = nullptr;
};


USTRUCT(BlueprintType)
struct FNaiveStateMachineInstance
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FNaiveStateMachineHandle InstanceHandle;
	
	UPROPERTY()
	UNaiveStateMachine* StateMachineAsset = nullptr;

	UPROPERTY()
	FName UniqueName = NAME_None;

	UPROPERTY()
	TArray<FNaiveStateInstance> ActiveStateLayerStack;

	// State Machine Stack From Bottom to Top, the name will be : Layer1.Layer2.Layer2.....
	FName ActiveStateName = NAME_None;
	
	UPROPERTY()
	class UWorld* FastWorld = nullptr;

	UPROPERTY()
	class UObject* Owner = nullptr;

	FNaiveStateChangeSignature OnStateChangedDelegate;
	FNaiveStateMachineStopSignature OnStoppedDelegate;

	TArray<uint8> InstanceMemory;

	TArrayView<const uint8> GetInstanceMemory() const { return InstanceMemory; }
};

using FNaiveEventQueue = TArray<FName>;
