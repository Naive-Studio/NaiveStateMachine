// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineManager.h"
#include "StateMachine/NaiveSMNodeBase.h"
#include "StateMachine/NaiveStateMachine.h"
#include "StateMachine/NaiveState.h"
#include "StateMachine/NaiveTransition.h"
#include "Stats/Stats.h"
#include "UObject/UObjectGlobals.h"


UNaiveStateMachineManager::UNaiveStateMachineManager(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{

}

FNaiveStateMachineHandle UNaiveStateMachineManager::StartStateMachine(const FNaiveRunStateMachineRequest& InRequest)
{
	if (InRequest.Template)
	{
		FNaiveStateMachineContext NewContext;
		UNaiveStateMachine* Template = InRequest.Template;

		const FName& InstanceTemplateName = Template->GetFName();
		NewContext.InstanceHandle = FNaiveStateMachineHandle::GenerateHandle();
		NewContext.Owner = InRequest.Owner;
		NewContext.FastWorld = InRequest.World;
		NewContext.UniqueName = InstanceTemplateName;
		NewContext.StateMachineAsset = Template;
		NewContext.StateChangeSignature = InRequest.OnStateChanged;
		NewContext.StoppedSignature = InRequest.OnStopped;
		
		GoToState(NewContext, NewContext.StateMachineAsset, NewContext.StateMachineAsset->GetDefaultState());
		
		RunningStateMachines.Add(NewContext.InstanceHandle, MoveTemp(NewContext));
		
		return NewContext.InstanceHandle;
	}

	return FNaiveStateMachineHandle::HANDLE_NONE;
}

void UNaiveStateMachineManager::StopStateMachine(const FNaiveStateMachineHandle& InHandle)
{
	if (const FNaiveStateMachineContext* LocalContext = RunningStateMachines.Find(InHandle))
	{
		LocalContext->StoppedSignature.ExecuteIfBound(LocalContext->Owner, InHandle);
		RunningStateMachines.Remove(InHandle);
	}
}

FName UNaiveStateMachineManager::GetActiveStateName(const FNaiveStateMachineHandle& InHandle)
{
	const FNaiveStateMachineContext* LocalContext = RunningStateMachines.Find(InHandle);
	return LocalContext ? LocalContext->ActiveStateName : NAME_None;
}

void UNaiveStateMachineManager::Tick(float DeltaTime)
{
	for (TMap<FNaiveStateMachineHandle, FNaiveStateMachineContext>::TIterator ContextItr = RunningStateMachines.CreateIterator(); ContextItr; ++ContextItr)
	{
		FNaiveStateMachineContext& StateMachineContext = ContextItr->Value;
		uint8* StateMachineMemory = StateMachineContext.InstanceMemory.GetData();
		
		bool bMeetCondition = false;
		for (auto& StateContext : StateMachineContext.ActiveStateContextStack)
		{
			if (IsValid(StateContext.StateNodeInstance))
			{
				uint8* NodeMemory = StateContext.GetInstanceMemory(StateMachineMemory);
				StateContext.StateNodeInstance->OnUpdate(StateMachineContext.Owner, NodeMemory, DeltaTime);
			}
			
 			for (const FNaiveTransitionContext& TransitionInstance : StateContext.ActiveTransitions)
 			{
 				UNaiveTransitionNode* TransitionObject = TransitionInstance.GetTransitionNodeInstance();

 				if(const FNaiveEventQueue* EventQueuePtr = ReceivedEventMap.Find(StateMachineContext.InstanceHandle))
 				{
 					bMeetCondition = TransitionInstance.IsListeningEvents(*EventQueuePtr);
 				}
 				
 				uint8* NodeMemory = TransitionInstance.GetInstanceMemory(StateMachineMemory);
 				bMeetCondition |= TransitionObject && TransitionObject->CheckCondition(StateMachineContext.Owner, NodeMemory, DeltaTime);
 				
 				if(bMeetCondition)
 				{
 					GoToState(StateMachineContext, StateContext.OwnerStateMachine, TransitionInstance.GetNextState());
 					break;
 				}
 			}
		}
	}

	ReceivedEventMap.Reset();
}

ETickableTickType UNaiveStateMachineManager::GetTickableTickType() const
{
	return (HasAnyFlags(RF_ClassDefaultObject))
		? ETickableTickType::Never
		: ETickableTickType::Always;
}

TStatId UNaiveStateMachineManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UNaiveStateMachineManager, STATGROUP_Tickables);
}

void UNaiveStateMachineManager::GoToState(FNaiveStateMachineContext& StateMachineContext, UNaiveStateMachine* InStateMachine, const FName& NewState)
{
	const FName OldStateName = StateMachineContext.ActiveStateName;
	uint8* StateMachineMemory = StateMachineContext.InstanceMemory.GetData();
	
	UNaiveStateMachine* StateMachineAsset = InStateMachine;
	
	if (StateMachineContext.ActiveStateContextStack.Num())
	{
		//Clear active state stack
		while (StateMachineContext.ActiveStateContextStack.Num() > 0 &&
			StateMachineContext.ActiveStateContextStack.Top().OwnerStateMachine != InStateMachine)
		{
			const FNaiveStateContext& StateContext = StateMachineContext.ActiveStateContextStack.Pop();
			if (StateContext.StateNodeInstance)
			{
				uint8* NodeMemory = StateContext.GetInstanceMemory(StateMachineMemory);
				StateContext.StateNodeInstance->OnExit(StateMachineContext.Owner, NodeMemory);
			}
		}

		const FNaiveStateContext& Context = StateMachineContext.ActiveStateContextStack.Pop();
		if (Context.StateNodeInstance)
		{
			uint8* NodeMemory = Context.GetInstanceMemory(StateMachineMemory);
			Context.StateNodeInstance->OnExit(StateMachineContext.Owner, NodeMemory);
		}
		
		StateMachineAsset = Context.OwnerStateMachine;
	}
	
	if(IsValid(StateMachineAsset))
	{
		int32 MemorySize = 0;
		TArray<UNaiveStateNode*> NewStateNodeLists;
		InnerGoToState(StateMachineContext, StateMachineAsset, NewState,MemorySize);
		StateMachineContext.InstanceMemory.SetNumZeroed(MemorySize);

		for(FNaiveStateContext& NewStateInstance : StateMachineContext.ActiveStateContextStack)
		{
			if(NewStateInstance.StateNodeInstance)
			{
				uint8* NodeMemory = NewStateInstance.GetInstanceMemory(StateMachineContext.InstanceMemory.GetData());
				NewStateInstance.StateNodeInstance->OnEnter(StateMachineContext.Owner,NodeMemory);
			}
		}

		StateMachineContext.ActiveStateName = BuildStateName(StateMachineContext.ActiveStateContextStack);
		
		if(StateMachineContext.StateChangeSignature.IsBound())
		{
			StateMachineContext.StateChangeSignature.Execute(OldStateName, StateMachineContext.ActiveStateName);
		}
	}
}

void UNaiveStateMachineManager::InnerGoToState(FNaiveStateMachineContext& StatMachineContext,
												UNaiveStateMachine* InStateMachine,
												FName NewState,int32& InMemorySize)
{
	const TMap<FName, FNaiveStateConfig>& LocalStateContextMap = InStateMachine->GetStateContexts();
	const FNaiveStateConfig& StateTemplate = LocalStateContextMap.FindChecked(NewState);
	
	auto CreateSMNode = [this, &StatMachineContext](UClass* InClass, bool& bInstanced, int32& MemoryOffset, int32& MemorySize, bool bForceInstance = false)
	{
		UNaiveSMNodeBase* NewStateBaseObject = nullptr;
		if (InClass)
		{
			UNaiveSMNodeBase* CDO = GetMutableDefault<UNaiveSMNodeBase>(InClass);
			NewStateBaseObject = CDO;
			if(bForceInstance || CDO->HasInstance())
			{
				NewStateBaseObject = NewObject<UNaiveSMNodeBase>(this, InClass);

				bInstanced = true;
			}

			if(!bInstanced)
			{
				MemoryOffset = MemorySize;
				MemorySize += NewStateBaseObject->GetInstanceMemorySize();
			}
			
			NewStateBaseObject->Initialize(StatMachineContext.Owner, StatMachineContext.FastWorld);
		}
		
		return NewStateBaseObject;
	};
	
	FNaiveStateContext StateInstance;
	StateInstance.StateName = NewState;
	StateInstance.OwnerStateMachine = InStateMachine;
	for (auto& Transition : StateTemplate.Transitions)
	{
		FNaiveTransitionContext TransitionInstance;
		TransitionInstance.SetTransitionConfig(&Transition);
		TransitionInstance.SetTransitionNodeInstance(Cast<UNaiveTransitionNode>(CreateSMNode(Transition.TransitionTemplate,TransitionInstance.bInstanced,TransitionInstance.MemoryOffset,InMemorySize)));
		StateInstance.ActiveTransitions.Add(MoveTemp(TransitionInstance));
	}

	if (StateTemplate.StateTemplate)
	{
		StateInstance.StateNodeInstance = Cast<UNaiveStateNode>(CreateSMNode(StateTemplate.StateTemplate,StateInstance.bInstanced,StateInstance.MemoryOffset,InMemorySize));
		check(StateInstance.StateNodeInstance);
	}
	StatMachineContext.ActiveStateContextStack.Push(MoveTemp(StateInstance));

	if (StateTemplate.SubStateMachineAsset)
	{
		FName DefaultSubState = StateTemplate.SubStateMachineAsset->GetDefaultState();
		bool bInstanced;
		int32 TempMemoryOffset;
		int32 TempMemorySize;

		//Check the sub-entry transition immediately
		for (const FNaiveTransitionConfig& Context : StateTemplate.SubStateMachineAsset->GetEntryTransitions())
		{
			UNaiveTransitionNode* NewTransitionObj = Cast<UNaiveTransitionNode>(CreateSMNode(Context.TransitionTemplate,bInstanced,TempMemoryOffset,TempMemorySize,true));
			if (NewTransitionObj && NewTransitionObj->CheckCondition(StatMachineContext.Owner,nullptr,0.0f))
			{
				DefaultSubState = Context.NextSate;
				break;
			}
		}

		const bool bInvalidDefaultState = DefaultSubState.IsNone();
		if (!bInvalidDefaultState)
		{
			InnerGoToState(StatMachineContext, StateTemplate.SubStateMachineAsset, DefaultSubState, InMemorySize);
		}
	}
}

FName UNaiveStateMachineManager::BuildStateName(const TArray<FNaiveStateContext>& StateStack)
{
	if (StateStack.Num()>0)
	{
		FString LayeredStateName = StateStack[0].StateName.ToString();
		for (int32 Index = 1; Index < StateStack.Num(); ++Index)
		{
			LayeredStateName += UNaiveStateMachine::StateNameJointMark;
			LayeredStateName += StateStack[Index].StateName.ToString();
		}

		return *LayeredStateName;
	}
	
	return NAME_None;
}

UNaiveStateNode* UNaiveStateMachineManager::GetActiveState(const FNaiveStateMachineHandle& InHandle)
{
	FNaiveStateMachineContext* LocalContext = RunningStateMachines.Find(InHandle);
	return LocalContext ? LocalContext->ActiveStateContextStack.Top().StateNodeInstance : nullptr;
}

void UNaiveStateMachineManager::PostEventToStateMachine(const FNaiveStateMachineHandle& InHandle, const FName& InEventName)
{
	FNaiveEventQueue& Events = ReceivedEventMap.FindOrAdd(InHandle);
	Events.Add(InEventName);
}
