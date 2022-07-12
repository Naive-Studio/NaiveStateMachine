#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "SGraphPin.h"

class NAIVESTATEMACHINEEDITOR_API SNaiveStateMachineGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SNaiveStateMachineGraphPin) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	//~ End SGraphPin Interface
};
