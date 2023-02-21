// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineManager.h"
#include "StateMachine/NaiveStateMachineObjectBase.h"
#include "StateMachine/NaiveStateMachine.h"
#include "StateMachine/NaiveStateNode.h"
#include "StateMachine/NaiveTransitionNode.h"
#include "Stats/Stats.h"
#include "UObject/UObjectGlobals.h"


UNaiveStateMachineManager::UNaiveStateMachineManager(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{

}

void UNaiveStateMachineManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UNaiveStateMachineManager::Deinitialize()
{
	Super::Deinitialize();
}

FNaiveStateMachineHandle UNaiveStateMachineManager::StartStateMachine(const FNaiveRunStateMachineRequest& InRequest)
{
	if (InRequest.Template)
	{
		FNaiveStateMachineContext NewContext;
		LoadStateMachine(NewContext, InRequest);
		GoToState(NewContext, NewContext.StateMachineAsset, NewContext.StateMachineAsset->GetDefaultState());
		
		RunningStateMachines.Add(NewContext.InstanceHandle, MoveTemp(NewContext));
		
		return NewContext.InstanceHandle;
	}

	return FNaiveStateMachineHandle::HANDLE_NONE;
}

void UNaiveStateMachineManager::LoadStateMachine(FNaiveStateMachineContext& InOutContext, const FNaiveRunStateMachineRequest& InRequest)
{
	UNaiveStateMachine* Template = InRequest.Template;

	const FName& InstanceTemplateName = Template->GetFName();
	InOutContext.InstanceHandle = FNaiveStateMachineHandle::GenerateHandle();
	InOutContext.Owner = InRequest.Owner;
	InOutContext.FastWorld = InRequest.World;
	InOutContext.UniqueName = InstanceTemplateName;
	InOutContext.StateMachineAsset = Template;
	InOutContext.StateChangeSignature = InRequest.OnStateChanged;
	InOutContext.StoppedSignature = InRequest.OnStopped;
}

void UNaiveStateMachineManager::StopStateMachine(const FNaiveStateMachineHandle& InHandle)
{
	if (FNaiveStateMachineContext* LocalContext = RunningStateMachines.Find(InHandle))
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
 				UNaiveTransitionNode* TransitionObject = TransitionInstance.TransitionObject;
 				if (TransitionInstance.ObservingEvents.Num() > 0 && PendingForwardEvents.Num() > 0)
 				{
					if (const FNaiveEventQueue* EventQueue = PendingForwardEvents.Find(StateMachineContext.InstanceHandle))
					{
						const TArray<FName>& Events = *EventQueue;
						for (auto& Event : Events)
						{
							if (TransitionInstance.ObservingEvents.Find(Event)!=INDEX_NONE)
							{
								bMeetCondition = true;
								break;
							}
						}
					}
 				}

 				uint8* NodeMemory = TransitionInstance.GetInstanceMemory(StateMachineMemory);
 				if(bMeetCondition ||
 					(TransitionObject && TransitionObject->CheckCondition(StateMachineContext.Owner, NodeMemory, DeltaTime)))
 				{
 					GoToState(StateMachineContext, StateContext.OwnerStateMachine, TransitionInstance.NextState);
 					break;
 				}
 			}
		}
	}

	PendingForwardEvents.Reset();
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
	
	auto CreateStateObject = [this, &StatMachineContext](UClass* InClass, bool& bInstanced, int32& MemoryOffset, int32& MemorySize, bool bForceInstance = false)
	{
		UNaiveStateMachineObjectBase* NewStateBaseObject = nullptr;
		if (InClass)
		{
			UNaiveStateMachineObjectBase* CDO = GetMutableDefault<UNaiveStateMachineObjectBase>(InClass);
			NewStateBaseObject = CDO;
			if(bForceInstance || CDO->HasInstance())
			{
				NewStateBaseObject = NewObject<UNaiveStateMachineObjectBase>(this, InClass);

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
	for (const FNaiveTransitionConfig& Transition : StateTemplate.Transitions)
	{
		FNaiveTransitionContext TransitionInstance;
		TransitionInstance.NextState = Transition.NextSate;
		if(Transition.bEventDriven)
		{
			TransitionInstance.ObservingEvents = Transition.ObservingEvents;
		}
		TransitionInstance.TransitionObject = Cast<UNaiveTransitionNode>(CreateStateObject(Transition.TransitionTemplate,TransitionInstance.bInstanced,TransitionInstance.MemoryOffset,InMemorySize));
		StateInstance.ActiveTransitions.Add(MoveTemp(TransitionInstance));
	}

	if (StateTemplate.StateTemplate)
	{
		StateInstance.StateNodeInstance = Cast<UNaiveStateNode>(CreateStateObject(StateTemplate.StateTemplate,StateInstance.bInstanced,StateInstance.MemoryOffset,InMemorySize));
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
			UNaiveTransitionNode* NewTransitionObj = Cast<UNaiveTransitionNode>(CreateStateObject(Context.TransitionTemplate,bInstanced,TempMemoryOffset,TempMemorySize,true));
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

void UNaiveStateMachineManager::SendEventToTransition(const FNaiveStateMachineHandle& InHandle, const FName& InEventName)
{
	FNaiveEventQueue& Events = PendingForwardEvents.FindOrAdd(InHandle);
	Events.Add(InEventName);
}
