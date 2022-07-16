// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveSubStateMachineGraph.h"
#include "NaiveSubStateMachineGraphSchema.h"
#include "NaiveStateMachineNode_Entry.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineNode_State.h"
#include "NaiveStateMachineNode_SubEntry.h"
#include "NaiveSubStateMachineGraphSchema.h"
#include "StateMachine/NaiveStateMachine.h"

UNaiveSubStateMachineGraph::UNaiveSubStateMachineGraph()
{
	Schema = UNaiveSubStateMachineGraphSchema::StaticClass();
}

void UNaiveSubStateMachineGraph::UpdateAsset(int32 UpdateFlags /*= 0*/)
{
	if (ParentGraph)
	{
		ParentGraph->UpdateAsset(UpdateFlags);
	}
}
