// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NaiveSMNodeBase.generated.h"

USTRUCT()
struct FNaiveNodeInstanceContext
{
	GENERATED_USTRUCT_BODY()

	bool bInstanced = false;
	
	int32 MemoryOffset = -1;
	
	FORCEINLINE uint8* GetInstanceMemory(uint8* StateMachineMemory) const
	{
		return bInstanced ? nullptr :
		(StateMachineMemory != nullptr ? (StateMachineMemory + MemoryOffset) : nullptr);
	}
};

/**
 * 
 */
UCLASS(abstract)
class NAIVESTATEMACHINE_API UNaiveSMNodeBase : public UObject
{
	GENERATED_BODY()

public:
	UNaiveSMNodeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Initialize(UObject* InOwnerObject, UWorld* InWorld);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SendEventToMachine(int32 InHandler, FName InMessage);

	//
	virtual class UWorld* GetWorld() const override;
	virtual void BeginDestroy() override;
	//

	/** sets bCreateNodeInstance flag, do NOT call this function on already pushed tree instance! */
	void ForceInstancing(bool bEnable);

	/** @return true if node wants to be instanced */
	bool HasInstance() const;

	virtual uint16 GetInstanceMemorySize() const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void OnInitialized(UObject* InOwner);
	
public:
	
	uint8 bCreateNodeInstance : 1;
	
protected:
	class UWorld* FastWorld  = nullptr;
};
