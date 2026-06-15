// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NBChatInput.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"

void UNBChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInput::SetNotificationMessage(const FText& InNotificationText, float PrintTime)
{
	Text_Notification->SetText(InNotificationText);
	GetWorld()->GetTimerManager().SetTimer(
		Handle_NotificationTimer,
		FTimerDelegate::CreateLambda([this]() 
			{
				Text_Notification->SetText(FText::GetEmpty());
			}
		),
		PrintTime,
		false
	);
}

void UNBChatInput::PushGuessLog(const FString& InText)
{
	GuessLog.Add(InText);
	if (GuessLog.Num() > 10)
	{
		GuessLog.RemoveAt(0);
	}
	PrintGuessLog();
}

void UNBChatInput::ClearGuessLog()
{
	GuessLog.Empty();
	PrintGuessLog();
}

void UNBChatInput::SetCountText(const int32& CurrentCount, const int32& MaxCount)
{
	Text_Count->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCount, MaxCount)));
}

void UNBChatInput::PrintGuessLog()
{
	FString TotalLog = TEXT("");
	for (int32 i = 0; i < GuessLog.Num(); i++)
	{
		TotalLog.Append(GuessLog[i] + TEXT("\n"));
	}
	Text_GuessLog->SetText(FText::FromString(TotalLog));
}

void UNBChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			ANBPlayerController* OwningNBPlayerController = Cast<ANBPlayerController>(OwningPlayerController);
			OwningNBPlayerController->SetChatMessageString(Text.ToString());

			EditableTextBox_ChatInput->SetText(FText());
		}
	}
}
