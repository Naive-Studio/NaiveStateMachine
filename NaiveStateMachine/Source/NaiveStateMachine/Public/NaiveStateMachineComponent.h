// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NaiveStateMachineTypes.h"
#include "NaiveStateMachineComponent.generated.h"

class FName;
class UNaiveStateMachine;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNaiveStateChangeMulticastSignature,FName, OldState, FName, NewState);


UCLASS( ClassGroup=(AI), meta=(BlueprintSpawnableComponent) )
class NAIVESTATEMACHINE_API UNaiveStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNaiveStateMachineComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "NaiveStateMachine")
	void SendEvent(FName InEvent);

	UFUNCTION(BlueprintCallable, Category = "NaiveStateMachine")
	void RunStateMachine(UNaiveStateMachine* InStateMachine);

	UFUNCTION(BlueprintCallable, Category = "NaiveStateMachine")
	void StopStateMachine();

	UPROPERTY(BlueprintAssignable)
	FNaiveStateChangeMulticastSignature OnStateChangeEvent;

protected:
	virtual void OnStateChanged(FName OldState, FName NewState);

protected:
	UPROPERTY(EditAnywhere)
	UNaiveStateMachine* DefaultStateMachine;

	FNaiveStateMachineHandle StateMachineHandle;
};
