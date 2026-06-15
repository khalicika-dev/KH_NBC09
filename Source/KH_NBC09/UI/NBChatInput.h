// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NBChatInput.generated.h"

class UEditableTextBox;
class UTextBlock;
/**
 * 
 */
UCLASS()
class KH_NBC09_API UNBChatInput : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetNotificationMessage(const FText& InNotificationText, float PrintTime);

	void PushGuessLog(const FString& InText);
	void ClearGuessLog();

	void SetCountText(const int32& CurrentCount, const int32& MaxCount);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;


protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	void PrintGuessLog();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Notification;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_GuessLog;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Count;

	TArray<FString> GuessLog;

	FTimerHandle Handle_NotificationTimer;
};
