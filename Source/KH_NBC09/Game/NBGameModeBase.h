// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBPlayerController;

/**
 * 
 */
UENUM()
enum class PrintResult : uint8
{
	CHAT = 0,
	INVALID = 1,
	VALID = 2
};

UCLASS()
class KH_NBC09_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	PrintResult IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ANBPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount);
protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ANBPlayerController>> AllPlayerControllers;
};
