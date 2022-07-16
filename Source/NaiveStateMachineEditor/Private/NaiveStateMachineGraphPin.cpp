#include "NaiveStateMachineGraphPin.h"
#include "Widgets/Layout/SSpacer.h"

//Reference: D:\Unreal Engine\UE_4.21\Engine\Source\Editor\AnimationBlueprintEditor\Private\AnimationStateNodes\SGraphNodeAnimState.cpp

void SNaiveStateMachineGraphPin::Construct(const FArguments & InArgs, UEdGraphPin * InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	CachedImg_Pin_Background = FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.Background"));
	CachedImg_Pin_BackgroundHovered = FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.BackgroundHovered"));

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SNaiveStateMachineGraphPin::GetPinBorder)
		.BorderBackgroundColor(this, &SNaiveStateMachineGraphPin::GetPinColor)
		.OnMouseButtonDown(this, &SNaiveStateMachineGraphPin::OnPinMouseDown)
		.Cursor(this, &SNaiveStateMachineGraphPin::GetPinCursor)
	);
}

TSharedRef<SWidget> SNaiveStateMachineGraphPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}