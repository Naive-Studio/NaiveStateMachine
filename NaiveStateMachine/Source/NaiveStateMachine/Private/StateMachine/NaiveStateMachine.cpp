// Fill out your copyright notice in the Description page of Project Settings.


#include"StateMachine/NaiveStateMachine.h"

#include "StateMachine/NaiveStateNode.h"
#include "StateMachine/NaiveTransitionNode.h"

class FName;

UNaiveStateMachine::UNaiveStateMachine()
{
	
}

void UNaiveStateMachine::PostInitProperties()
{
	Super::PostInitProperties();

	StateMachineName = GetFName();
}

void UNaiveStateMachine::SetDefaultState(const FName& InState)
{
	DefaultState = InState;
}

const FName& UNaiveStateMachine::GetDefaultState() const
{
	return DefaultState;
}

const TCHAR UNaiveStateMachine::StateNameJointMark = '.';
