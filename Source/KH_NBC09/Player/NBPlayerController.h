// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UNBChatInput;
class UUserWidget;
/**
 * 
 */
UCLASS()
class KH_NBC09_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ANBPlayerController();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	bool InitMainWidget();

	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintNotificationMessage(const FText& InNotificationText, float PrintTime = 5.f);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
	UFUNCTION(Client, Reliable)
	void ClientRPCAddGuessLog(const FString& GuessLog);
	UFUNCTION(Client, Reliable)
	void ClientRPCSetCountText(const int32& CurrentCount, const int32& MaxCount);
	UFUNCTION(Client, Reliable)
	void ClientRPCClearGuessLog();
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNBChatInput> ChatInputWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UNBChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;
};
