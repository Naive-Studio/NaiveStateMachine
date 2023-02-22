// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Blueprint.h"
#include "StateMachine/NaiveState.h"
#include "NaiveStateMachine.generated.h"


class UNaiveStateNode;
class UNaiveTransitionNode;

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
	class UNaiveStateMachine* Template = nullptr;

	UPROPERTY(EditAnywhere)
	UObject* Owner = nullptr;

	UPROPERTY(EditAnywhere)
	UWorld* World = nullptr;

	FNaiveStateChangeSignature OnStateChanged;
	
	FNaiveStateMachineStopSignature OnStopped;
};

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

	FORCEINLINE bool IsValid() const
	{
		return Handle != INDEX_NONE;
	}

	FORCEINLINE void Invalidate()
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

	FORCEINLINE FString ToString() const
	{
		return FString::Printf(TEXT("%d"), Handle);
	}

	FORCEINLINE int32 ToInt() const
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

USTRUCT(BlueprintType)
struct FNaiveStateMachineContext
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FNaiveStateMachineHandle InstanceHandle;
	
	UPROPERTY()
	UNaiveStateMachine* StateMachineAsset = nullptr;

	UPROPERTY()
	TArray<FNaiveStateContext> ActiveStateContextStack;
	
	UPROPERTY()
	class UWorld* FastWorld = nullptr;

	UPROPERTY()
	class UObject* Owner = nullptr;

	// State Machine Stack From Bottom to Top, the name will be : Layer1.Layer2.Layer3.....
	FName ActiveStateName = NAME_None;

	FName UniqueName = NAME_None;

	FNaiveStateChangeSignature StateChangeSignature;
	
	FNaiveStateMachineStopSignature StoppedSignature;
	
	TArray<uint8> InstanceMemory;
};

/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveStateMachine : public UDataAsset
{
	GENERATED_BODY()

public:
	UNaiveStateMachine();
	virtual  void PostInitProperties() override;

	TMap<FName, FNaiveStateConfig>& GetStateContextsMutable() { return RegisteredStateMachineContext; }
	const TMap<FName, FNaiveStateConfig>& GetStateContexts() const { return RegisteredStateMachineContext; }

	void SetDefaultState(const FName& InState);
	const FName& GetDefaultState() const;

	TArray<FNaiveTransitionConfig>& GetEntryTransitionsMutable() { return SubEntryTransitions; }
	const TArray<FNaiveTransitionConfig>& GetEntryTransitions() const { return SubEntryTransitions; }
public:
#if WITH_EDITORONLY_DATA
	/** Graph for state machine */
	UPROPERTY()
	UEdGraph* EdGraph;

	/** Info about the graphs we last edited */
	UPROPERTY()
	TArray<FEditedDocumentInfo> LastEditedDocuments;
#endif

	static const TCHAR StateNameJointMark;

protected:
	UPROPERTY()
	TMap<FName, FNaiveStateConfig> RegisteredStateMachineContext;

	UPROPERTY()
	TArray<FNaiveTransitionConfig> SubEntryTransitions;

	UPROPERTY()
	FName DefaultState = NAME_None;

	UPROPERTY()
	FName StateMachineName;
};
