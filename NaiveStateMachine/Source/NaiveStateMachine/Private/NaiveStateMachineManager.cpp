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
		FNaiveStateMachineInstance NewContext;
		LoadStateMachine(NewContext, InRequest);
		SwitchState(NewContext, NewContext.StateMachineAsset, NewContext.StateMachineAsset->GetDefaultState());
		
		RunningStateMachines.Add(NewContext.InstanceHandle, MoveTemp(NewContext));
		
		return NewContext.InstanceHandle;
	}

	return FNaiveStateMachineHandle::HANDLE_NONE;
}

void UNaiveStateMachineManager::LoadStateMachine(FNaiveStateMachineInstance& InOutContext, const FNaiveRunStateMachineRequest& InRequest)
{
	UNaiveStateMachine* Template = InRequest.Template;

	const FName& InstanceTemplateName = Template->GetFName();
	InOutContext.InstanceHandle = FNaiveStateMachineHandle::GenerateHandle();
	InOutContext.Owner = InRequest.Owner;
	InOutContext.FastWorld = InRequest.World;
	InOutContext.UniqueName = InstanceTemplateName;
	InOutContext.StateMachineAsset = Template;
	InOutContext.OnStateChangedDelegate = InRequest.Delegate;
	InOutContext.OnStoppedDelegate = InRequest.OnStoppedDelegate;
}

void UNaiveStateMachineManager::StopStateMachine(const FNaiveStateMachineHandle& InHandle)
{
	if (FNaiveStateMachineInstance* LocalContext = RunningStateMachines.Find(InHandle))
	{
		LocalContext->OnStoppedDelegate.ExecuteIfBound(LocalContext->Owner, InHandle);
		RunningStateMachines.Remove(InHandle);
	}
}

FName UNaiveStateMachineManager::GetActiveStateName(const FNaiveStateMachineHandle& InHandle)
{
	const FNaiveStateMachineInstance* LocalContext = RunningStateMachines.Find(InHandle);
	return LocalContext ? LocalContext->ActiveStateName : NAME_None;
}

void UNaiveStateMachineManager::Tick(float DeltaTime)
{
	for (TMap<FNaiveStateMachineHandle, FNaiveStateMachineInstance>::TIterator ContextItr = RunningStateMachines.CreateIterator(); ContextItr; ++ContextItr)
	{
		FNaiveStateMachineInstance& StateMachineInstance = ContextItr->Value;
		uint8* InstanceMemoryData = StateMachineInstance.InstanceMemory.GetData();
		
		bool bMeetCondition = false;
		for (auto& StateInstance : StateMachineInstance.ActiveStateLayerStack)
		{
			if (IsValid(StateInstance.StateNodeInstance))
			{
				uint8* NodeMemory = StateInstance.bInstanced?nullptr:(InstanceMemoryData!=nullptr?InstanceMemoryData+StateInstance.GetInstanceMemoryOffset():nullptr);
				StateInstance.StateNodeInstance->OnUpdate(StateMachineInstance.Owner, NodeMemory, DeltaTime);
			}
			
 			for (const FNaiveTransitionInstance& TransitionInstance : StateInstance.ActiveTransitions)
 			{
 				UNaiveTransitionNode* TransitionObject = TransitionInstance.TransitionObject;
 				if (TransitionInstance.ObservingEvents.Num() > 0 && PendingForwardEvents.Num() > 0)
 				{
					if (const FNaiveEventQueue* EventQueue = PendingForwardEvents.Find(StateMachineInstance.InstanceHandle))
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

 				uint8* NodeMemory = TransitionInstance.bInstanced?nullptr:(InstanceMemoryData!=nullptr?InstanceMemoryData+TransitionInstance.GetInstanceMemoryOffset():nullptr);
 				if(bMeetCondition ||
 					(TransitionObject && TransitionObject->CheckCondition(StateMachineInstance.Owner, NodeMemory, DeltaTime)))
 				{
 					SwitchState(StateMachineInstance, StateInstance.OwnerStateMachine, TransitionInstance.NextSate);
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

void UNaiveStateMachineManager::SwitchState(FNaiveStateMachineInstance& StateMachineContext, UNaiveStateMachine* InStateMachine, const FName& NewState)
{
	const FName OldStateName = StateMachineContext.ActiveStateName;
	uint8* InstanceMemoryData = StateMachineContext.InstanceMemory.GetData();
	
	UNaiveStateMachine* StateMachineAsset = InStateMachine;
	
	if (StateMachineContext.ActiveStateLayerStack.Num())
	{
		//Clear active state stack
		while (StateMachineContext.ActiveStateLayerStack.Num() > 0 &&
			StateMachineContext.ActiveStateLayerStack.Top().OwnerStateMachine != InStateMachine)
		{
			const FNaiveStateInstance& StateContext = StateMachineContext.ActiveStateLayerStack.Pop();
			if (StateContext.StateNodeInstance)
			{
				uint8* NodeMemory = StateContext.bInstanced?nullptr:(InstanceMemoryData!=nullptr?InstanceMemoryData+StateContext.GetInstanceMemoryOffset():nullptr);
				StateContext.StateNodeInstance->OnExit(StateMachineContext.Owner, NodeMemory);
			}
		}

		const FNaiveStateInstance& Context = StateMachineContext.ActiveStateLayerStack.Pop();
		if (Context.StateNodeInstance)
		{
			uint8* NodeMemory = Context.bInstanced?nullptr:(InstanceMemoryData!=nullptr?InstanceMemoryData+Context.GetInstanceMemoryOffset():nullptr);
			Context.StateNodeInstance->OnExit(StateMachineContext.Owner, NodeMemory);
		}
		
		StateMachineAsset = Context.OwnerStateMachine;
	}
	
	if(IsValid(StateMachineAsset))
	{
		int32 MemorySize = 0;
		TArray<UNaiveStateNode*> NewStateNodeLists;
		InnerEnterState(StateMachineContext, StateMachineAsset, NewState,MemorySize);
		StateMachineContext.InstanceMemory.SetNumZeroed(MemorySize);

		for(FNaiveStateInstance& NewStateInstance : StateMachineContext.ActiveStateLayerStack)
		{
			if(NewStateInstance.StateNodeInstance)
			{
				uint8* NodeMemory = NewStateInstance.bInstanced ? nullptr :
				           (StateMachineContext.InstanceMemory.GetData()!=nullptr ?
				           	StateMachineContext.InstanceMemory.GetData()+NewStateInstance.GetInstanceMemoryOffset() :
				           	nullptr);
				
				NewStateInstance.StateNodeInstance->OnEnter(StateMachineContext.Owner,NodeMemory);
			}
		}

		StateMachineContext.ActiveStateName = MakeLayeredStateName(StateMachineContext.ActiveStateLayerStack);
		StateMachineContext.OnStateChangedDelegate.ExecuteIfBound(OldStateName, StateMachineContext.ActiveStateName);
	}
}

void UNaiveStateMachineManager::InnerEnterState(FNaiveStateMachineInstance& StatMachineContext,
												UNaiveStateMachine* InStateMachine,
												FName NewState,int32& InMemorySize)
{
	const TMap<FName, FNaiveState>& LocalStateContextMap = InStateMachine->GetStateContexts();
	const FNaiveState& StateTemplate = LocalStateContextMap.FindChecked(NewState);
	
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
	
	FNaiveStateInstance StateInstance;
	StateInstance.StateName = NewState;
	StateInstance.OwnerStateMachine = InStateMachine;
	for (const FNaiveTransition& Transition : StateTemplate.Transitions)
	{
		FNaiveTransitionInstance TransitionInstance;
		TransitionInstance.NextSate = Transition.NextSate;
		if(Transition.bEventDriven)
		{
			TransitionInstance.ObservingEvents = Transition.ObservingEvents;
		}
		TransitionInstance.TransitionObject = Cast<UNaiveTransitionNode>(CreateStateObject(Transition.TransitionTemplate,TransitionInstance.bInstanced,TransitionInstance.InstanceMemoryOffset,InMemorySize));
		StateInstance.ActiveTransitions.Add(MoveTemp(TransitionInstance));
	}

	if (StateTemplate.StateTemplate)
	{
		StateInstance.StateNodeInstance = Cast<UNaiveStateNode>(CreateStateObject(StateTemplate.StateTemplate,StateInstance.bInstanced,StateInstance.InstanceMemoryOffset,InMemorySize));
		check(StateInstance.StateNodeInstance);
	}
	StatMachineContext.ActiveStateLayerStack.Push(MoveTemp(StateInstance));

	if (StateTemplate.SubStateMachineAsset)
	{
		FName DefaultSubState = StateTemplate.SubStateMachineAsset->GetDefaultState();
		bool bInstanced;
		int32 TempMemoryOffset;
		int32 TempMemorySize;

		//Check the sub-entry transition immediately
		for (const FNaiveTransition& Context : StateTemplate.SubStateMachineAsset->GetEntryTransitions())
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
			InnerEnterState(StatMachineContext, StateTemplate.SubStateMachineAsset, DefaultSubState, InMemorySize);
		}
	}
}

FName UNaiveStateMachineManager::MakeLayeredStateName(const TArray<FNaiveStateInstance>& StateStack)
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
	FNaiveStateMachineInstance* LocalContext = RunningStateMachines.Find(InHandle);
	return LocalContext ? LocalContext->ActiveStateLayerStack.Top().StateNodeInstance : nullptr;
}

void UNaiveStateMachineManager::SendEventToTransition(const FNaiveStateMachineHandle& InHandle, const FName& InEventName)
{
	FNaiveEventQueue& Events = PendingForwardEvents.FindOrAdd(InHandle);
	Events.Add(InEventName);
}
