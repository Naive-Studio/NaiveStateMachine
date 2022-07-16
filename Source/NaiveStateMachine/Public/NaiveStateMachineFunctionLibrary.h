// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaiveStateMachineTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NaiveStateMachineFunctionLibrary.generated.h"

class UObject;
/**
 * 
 */
UCLASS()
class NAIVESTATEMACHINE_API UNaiveStateMachineFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="StateMachine", meta = (WorldContext = "ContextObject"))
	static FNaiveStateMachineHandle RunStateMachine(UObject* ContextObject, const FNaiveRunStateMachineRequest& InRequest);
	
	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (WorldContext = "ContextObject"))
	static void StopStateMachine(UObject* ContextObject, const FNaiveStateMachineHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (WorldContext = "ContextObject"))
	static FName GetActiveState(UObject* ContextObject, const FNaiveStateMachineHandle& InHandle);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (WorldContext = "ContextObject"))
	static class UNaiveStateMachineManager* GetStateMachineManager(UObject* ContextObject);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (WorldContext = "ContextObject"))
	static void SendEventToStateMachine(UObject* ContextObject, const FNaiveStateMachineHandle& InHandle, const FName& EventName);
};
