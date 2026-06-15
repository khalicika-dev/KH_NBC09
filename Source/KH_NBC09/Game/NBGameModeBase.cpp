// Fill out your copyright notice in the Description page of Project Settings.


#include "NBGameModeBase.h"
#include "Game/NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "EngineUtils.h"

void ANBGameModeBase::OnPostLogin(AController* NewPlayer)
{
    Super::OnPostLogin(NewPlayer);

	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(NBPlayerController) == true)
	{
		NBPlayerController->ClientRPCPrintNotificationMessage(FText::FromString(TEXT("Connected to the game server.")));

		AllPlayerControllers.Add(NBPlayerController);

		ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPS) == true)
		{
			NBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
			NBPlayerController->ClientRPCSetCountText(NBPS->CurrentGuessCount, NBPS->MaxGuessCount);
		}

		ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameStateBase) == true)
		{
			NBGameStateBase->MulticastRPCBroadcastLoginMessage(NBPS->PlayerNameString);
		}
	}
}

FString ANBGameModeBase::GenerateSecretNumber()
{
	// 정답은 서버에만 존재하는 GameModeBase에서 작성한다.
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}
	UE_LOG(LogTemp, Error, TEXT("GenerateSecretNumber(): %s"), *Result);
	return Result;
}

PrintResult ANBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	PrintResult Result = PrintResult::CHAT;

	do {
		if (InNumberString.Len() != 3)
		{
			break;
		}
		Result = PrintResult::INVALID;
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}
			UniqueDigits.Add(C);
		}
		if (bIsUnique == false)
		{
			break;
		}

		if (UniqueDigits.Num() == 3)
		{
			Result = PrintResult::VALID;
		}
	} while (false);
	return Result;
}

FString ANBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ANBGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void ANBGameModeBase::PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	FString PlayerInfoString = TEXT("");
	PrintResult OutputType = IsGuessNumberString(InChatMessageString);

	ANBPlayerState* ChatPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(ChatPS) == true)
	{
		if (ChatPS->CurrentGuessCount >= ChatPS->MaxGuessCount)
		{
			OutputType = PrintResult::CHAT;
		}
	}

	if (OutputType == PrintResult::VALID)
	{
		bool EndGame = false;
		FString JudgeResultString = JudgeResult(SecretNumberString, InChatMessageString);

		IncreaseGuessCount(InChattingPlayerController);
		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPlayerController = *It;
			if (IsValid(NBPlayerController) == true)
			{
				if (IsValid(ChatPS) == true)
				{
					PlayerInfoString = ChatPS->GetPlayerInfoString();
					if (InChattingPlayerController == *It)
					{
						NBPlayerController->ClientRPCSetCountText(ChatPS->CurrentGuessCount, ChatPS->MaxGuessCount);
					}
				}
				FString CombinedMessageString = PlayerInfoString + TEXT(": ") + InChatMessageString + TEXT(" -> ") + JudgeResultString;
				NBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
				NBPlayerController->ClientRPCAddGuessLog(InChatMessageString + TEXT(" -> ") + JudgeResultString);
				
				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				EndGame = JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
		if (EndGame == true)
		{
			ResetGame();
		}
	}
	else
	{
		for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
		{
			ANBPlayerController* NBPlayerController = *It;
			if (IsValid(NBPlayerController) == true)
			{
				if (IsValid(ChatPS) == true)
				{
					PlayerInfoString = ChatPS->GetPlayerInfoString();
				}
				FString CombinedMessageString = PlayerInfoString + TEXT(": ") + InChatMessageString;
				NBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

				if (*It == InChattingPlayerController && OutputType == PrintResult::INVALID)
				{
					NBPlayerController->ClientRPCAddGuessLog(InChatMessageString + TEXT(" -> Invalid"));
				}
			}
		}
	}
}

void ANBGameModeBase::IncreaseGuessCount(ANBPlayerController* InChattingPlayerController)
{
	ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPS) == true)
	{
		NBPS->CurrentGuessCount++;
		UE_LOG(LogTemp, Warning, TEXT("Count:%d"), NBPS->CurrentGuessCount);
	}
}

void ANBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (TActorIterator<ANBPlayerController> It(GetWorld()); It; ++It)
	{
		ANBPlayerController* NBPlayerController = *It;
		if (IsValid(NBPlayerController) == true)
		{
			NBPlayerController->ClientRPCClearGuessLog();

			ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
			if (IsValid(NBPS) == true)
			{
				NBPS->CurrentGuessCount = 0;
				NBPlayerController->ClientRPCSetCountText(NBPS->CurrentGuessCount, NBPS->MaxGuessCount);
			}
		}
	}
}

bool ANBGameModeBase::JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	bool bEnd = false;
	if (InStrikeCount == 3)
	{
		ANBPlayerState* NBPS = InChattingPlayerController->GetPlayerState<ANBPlayerState>();
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			if (IsValid(NBPS) == true)
			{
				FString CombinedMessageString = NBPS->PlayerNameString + TEXT(" has won the game.");
				NBPlayerController->ClientRPCPrintNotificationMessage(FText::FromString(CombinedMessageString));

				bEnd = true;
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& NBPlayerController : AllPlayerControllers)
		{
			ANBPlayerState* NBPS = NBPlayerController->GetPlayerState<ANBPlayerState>();
			if (IsValid(NBPS) == true)
			{
				if (NBPS->CurrentGuessCount < NBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}
		if (bIsDraw == true)
		{
			for (const auto& NBPlayerController : AllPlayerControllers)
			{
				NBPlayerController->ClientRPCPrintNotificationMessage(FText::FromString(TEXT("Draw...")));

				bEnd = true;
			}
		}
	}

	return bEnd;
}
