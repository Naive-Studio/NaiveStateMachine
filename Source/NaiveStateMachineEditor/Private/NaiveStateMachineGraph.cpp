// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineGraph.h"
#include "NaiveStateMachineGraphSchema.h"
#include "NaiveStateMachineNode_Entry.h"
#include "NaiveStateMachineNode_SubEntry.h"
#include "NaiveStateMachineNode_SubMachine.h"
#include "NaiveStateMachineNode_Transition.h"
#include "NaiveStateMachineNode_State.h"
#include "StateMachine/NaiveStateMachine.h"

UNaiveStateMachineGraph::UNaiveStateMachineGraph()
{
	Schema = UNaiveStateMachineGraphSchema::StaticClass();
}

void UNaiveStateMachineGraph::UpdateAsset(int32 UpdateFlags /*= 0*/)
{
	UNaiveStateMachine* StateMachine = Cast<UNaiveStateMachine>(GetOuter());
	check(StateMachine);

	TMap<FName, FNaiveStateConfig>& StateMachineContext = StateMachine->GetStateContextsMutable();
	TArray<FNaiveTransitionConfig>& SubEntryTransitions = StateMachine->GetEntryTransitionsMutable();
	StateMachineContext.Reset();
	SubEntryTransitions.Reset();
	
	for (UEdGraphNode* Node : Nodes)
	{
		if (Node->IsA(UNaiveStateMachineNode_Entry::StaticClass()))
		{
			const TArray<UEdGraphPin*>& LinkToNodes = Node->Pins[0]->LinkedTo;
			if (LinkToNodes.Num() > 0)
			{
				UNaiveStateMachineNode_State* DefaultStateNode = Cast<UNaiveStateMachineNode_State>(LinkToNodes[0]->GetOwningNode());

				if (!DefaultStateNode->IsA(UNaiveStateMachineNode_SubEntry::StaticClass()))
				{
					const FName& DefaultStateName = FName(*DefaultStateNode->GetNodeName());
					StateMachine->SetDefaultState(DefaultStateName);
					
					if (nullptr == StateMachineContext.Find(DefaultStateName))
					{
						FNaiveStateConfig NewContext;
						NewContext.StateTemplate = DefaultStateNode->GetTemplateClass();
						NewContext.SubStateMachineAsset = nullptr;
						StateMachineContext.Add(DefaultStateName, NewContext);
					}
				}
				else
				{
					StateMachine->SetDefaultState(NAME_None);
				}
			}
			else
			{
				break;
			}
		}
		else if(Node->IsA(UNaiveStateMachineNode_Transition::StaticClass()))
		{
			UNaiveStateMachineNode_Transition* Transition = Cast<UNaiveStateMachineNode_Transition>(Node);
			UNaiveStateMachineNode_State* PrevStateNode = Transition->GetPreviousState();
			UNaiveStateMachineNode_State* NextStateNode = Transition->GetNextState();

			if (PrevStateNode && NextStateNode)
			{
				const FName& NextStateName = FName(*NextStateNode->GetNodeName());

				FNaiveTransitionConfig NewTransitionContext;
				NewTransitionContext.NextSate = NextStateName;
				NewTransitionContext.TransitionTemplate = Transition->GetTemplateClass();
				NewTransitionContext.bEventDriven = Transition->bEventDriven;
				NewTransitionContext.ObservingEvents = Transition->ObservingEvents;

				if (PrevStateNode->IsA(UNaiveStateMachineNode_SubEntry::StaticClass()))
				{
					SubEntryTransitions.Add(NewTransitionContext);
				}
				else
				{
					const FName& PrevStateName = FName(*PrevStateNode->GetNodeName());
					
					FNaiveStateConfig& PrevContext = StateMachineContext.FindOrAdd(PrevStateName);
					FNaiveStateConfig& NextContext = StateMachineContext.FindOrAdd(NextStateName);

					PrevContext.StateTemplate = PrevStateNode->GetTemplateClass();
					PrevContext.Transitions.Add(NewTransitionContext);

					if (UNaiveStateMachineNode_SubMachine* PrevSubMachine = Cast<UNaiveStateMachineNode_SubMachine>(PrevStateNode))
					{
						PrevContext.SubStateMachineAsset = PrevSubMachine->StateMachineAsset;
					}

					NextContext.StateTemplate = NextStateNode->GetTemplateClass();
					if (UNaiveStateMachineNode_SubMachine* NextSubMachine = Cast<UNaiveStateMachineNode_SubMachine>(NextStateNode))
					{
						NextContext.SubStateMachineAsset = NextSubMachine->StateMachineAsset;
					}
					
				}
			}
		}
	}

	UAIGraph::UpdateAsset(UpdateFlags);
}
