// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveSMNodeBase.h"
#include "NaiveTransition.generated.h"

USTRUCT()
struct FNaiveTransitionConfig
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UNaiveTransitionNode> TransitionTemplate = nullptr;

	UPROPERTY(EditAnywhere)
	FName NextSate = NAME_None;

	UPROPERTY(EditAnywhere)
	bool bEventDriven = false;

	UPROPERTY(EditAnywhere)
	TArray<FName> ObservingEvents;
};

USTRUCT()
struct FNaiveTransitionContext : public FNaiveNodeInstanceContext
{
	GENERATED_USTRUCT_BODY()

public:
	FORCEINLINE const FName GetNextState() const
	{
		return (TransitionConfig != nullptr) ? TransitionConfig->NextSate : NAME_None;
	}

	FORCEINLINE UNaiveTransitionNode* GetTransitionNodeInstance() const
	{
		return TransitionNodeInstance;
	}

	bool IsListeningEvents(const TArray<FName>& InEvents) const;

	void SetTransitionConfig(const FNaiveTransitionConfig* InTransitionConfig);
	void SetTransitionNodeInstance(UNaiveTransitionNode* NodeInstance);
	
protected:
	const FNaiveTransitionConfig* TransitionConfig = nullptr;

	UPROPERTY(Transient)
	UNaiveTransitionNode* TransitionNodeInstance = nullptr;
};

/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveTransitionNode : public UNaiveSMNodeBase
{
	GENERATED_BODY()

public:
	UNaiveTransitionNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Initialize(UObject* InOwnerObject, UWorld* InWorld) override;
	
	virtual bool CheckCondition(UObject* OwnerObject, uint8* NodeMemory, float DeltaTime);
};
