// Fill out your copyright notice in the Description page of Project Settings.


#include "NaiveStateMachineTypes.h"

DEFINE_LOG_CATEGORY(LogSM);

FNaiveStateMachineHandle FNaiveStateMachineHandle::GenerateHandle()
{
	return FNaiveStateMachineHandle(NextHandle++);
}

FNaiveStateMachineHandle FNaiveStateMachineHandle::HANDLE_NONE = FNaiveStateMachineHandle();

int32 FNaiveStateMachineHandle::NextHandle = 0;
