// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if(GameState)
    {
        int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

        if(GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                1,
                60.f,
                FColor::Blue,
                FString::Printf(TEXT("Player in Game: %d"), NumberOfPlayers)
            );
        }

        APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
        if(PlayerState)
        {
            FString PlayerName = PlayerState->GetPlayerName();

            GEngine->AddOnScreenDebugMessage(
                1,
                60.f,
                FColor::Cyan,
                FString::Printf(TEXT("%s Join the Game"), *PlayerName)
            );
        }
        
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
    if(PlayerState)
    {
        FString PlayerName = PlayerState->GetPlayerName();

        GEngine->AddOnScreenDebugMessage(
            1,
            60.f,
            FColor::Cyan,
            FString::Printf(TEXT("%s has left the Game"), *PlayerName)
        );
    }
}
