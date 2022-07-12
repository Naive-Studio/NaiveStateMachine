// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIGraph.h"
#include "NaiveStateMachineGraph.generated.h"

/**
 * 
 */
UCLASS()
class UNaiveStateMachineGraph : public UAIGraph
{
	GENERATED_BODY()
public:
	UNaiveStateMachineGraph();

	virtual void UpdateAsset(int32 UpdateFlags = 0) override;
};
