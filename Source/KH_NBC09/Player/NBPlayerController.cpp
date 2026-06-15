// Fill out your copyright notice in the Description page of Project Settings.


#include "NBPlayerController.h"
#include "KH_NBC09.h"
#include "UI/NBChatInput.h"
#include "Kismet/GameplayStatics.h"
#include "Game/NBGameModeBase.h"
#include "Player/NBPlayerState.h"
#include "Net/UnrealNetwork.h"

ANBPlayerController::ANBPlayerController()
{
	bReplicates = true;
}

void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetInstance) == false)
	{
		InitMainWidget();
	}
}

void ANBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool ANBPlayerController::InitMainWidget()
{
	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UNBChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
			return true;
		}
	}
	return false;
}

void ANBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	
	if (IsLocalController() == true)
	{
		ANBPlayerState* NBPS = GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			ServerRPCPrintChatMessageString(InChatMessageString);
		}
	}
}

void ANBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	ChatFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ANBPlayerController::ClientRPCSetCountText_Implementation(const int32& CurrentCount, const int32& MaxCount)
{
	if (IsValid(ChatInputWidgetInstance) == true)
	{
		ChatInputWidgetInstance->SetCountText(CurrentCount, MaxCount);
	}
}

void ANBPlayerController::ClientRPCClearGuessLog_Implementation()
{
	if (IsValid(ChatInputWidgetInstance) == true)
	{
		ChatInputWidgetInstance->ClearGuessLog();
	}
}

void ANBPlayerController::ClientRPCAddGuessLog_Implementation(const FString& GuessLog)
{
	if (IsValid(ChatInputWidgetInstance) == true)
	{
		ChatInputWidgetInstance->PushGuessLog(GuessLog);
	}
}

void ANBPlayerController::ClientRPCPrintNotificationMessage_Implementation(const FText& InNotificationText, float PrintTime)
{
	bool bWidgetInstanceValid = false;
	if (IsValid(ChatInputWidgetInstance) == true)
	{
		bWidgetInstanceValid = true;
	}
	else
	{
		if (InitMainWidget())
		{
			bWidgetInstanceValid = true;
		}
	}
	if (bWidgetInstanceValid == true)
	{
		ChatInputWidgetInstance->SetNotificationMessage(InNotificationText, PrintTime);
	}
}

void ANBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ANBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	//서버에만 존재하는 게임모드에 전달
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ANBGameModeBase* NBGM = Cast<ANBGameModeBase>(GM);
		if (IsValid(NBGM) == true)
		{
			NBGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}
