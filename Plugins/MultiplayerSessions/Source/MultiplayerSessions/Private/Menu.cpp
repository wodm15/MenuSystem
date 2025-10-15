// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiPlayerSessionsSubsystem.h"
#include "Menu.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"



void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch , FString LobbyPath)
{
    PathToLobby = FString::Printf(TEXT("%s?Listen") , *LobbyPath);
    numPublicConnections = NumberOfPublicConnections;
    MatchType = TypeOfMatch;
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    //bIsFocusable = true;

    UWorld* World = GetWorld();
    if(World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if(PlayerController)
        {
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }

    if(MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
        MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSession);
        MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
        MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
    }
}

bool UMenu::Initialize()
{
    if(!Super::Initialize()) return false;

    if(HostButton)
    {
        HostButton->OnClicked.AddDynamic(this , &ThisClass::HostButtonClicked);
    }
    if(JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
    }

    return true;
}

void UMenu::NativeDestruct()
{
    MenuTearDown();
    Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bSucessful)
{
    if( bSucessful)
    {
        if(GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Yellow,
                FString(TEXT("Session create sucess"))
            );
        }
        
        UWorld* World = GetWorld();
        if(World)
        {
            World->ServerTravel(PathToLobby);
        }
    }
    else
    {
        if(GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                15.f,
                FColor::Red,
                FString(TEXT("Fail to Create Session"))
            );
        }
    }
    HostButton->SetIsEnabled(true);

}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
    if(MultiplayerSessionsSubsystem == nullptr) return;

    for(auto Result : SessionResults)
    {
        FString SettingValue;
        Result.Session.SessionSettings.Get(FName("MatchType"), SettingValue);
        if(SettingValue == MatchType)
        {
            MultiplayerSessionsSubsystem->JoinSession(Result);
            return;
        }
    }

	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if(Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if(SessionInterface.IsValid())
        {
            FString Address;
            SessionInterface->GetResolvedConnectString(NAME_GameSession , Address);

            
            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if(PlayerController)
            {
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }
        }
    }
    if(Result != EOnJoinSessionCompleteResult::Success)
    {
        JoinButton->SetIsEnabled(true);
    }
}

void UMenu::OnDestroySession(bool bSucessful)
{

}

void UMenu::OnStartSession(bool bSucessful)
{

}

void UMenu::HostButtonClicked()
{
    HostButton->SetIsEnabled(false);

    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Yellow,
            FString(TEXT("Host button clicked"))
        );
    }

    if(MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->CreateSession(numPublicConnections, MatchType);
    }

}

void UMenu::JoinButtonClicked()
{
    JoinButton->SetIsEnabled(false);
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            15.f,
            FColor::Yellow,
            FString(TEXT("Join button clicked"))
        );
    }

    if(MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->FindSessions(10000);
    }
}

void UMenu::MenuTearDown()
{
    RemoveFromParent();

    UWorld* World = GetWorld();
    if(World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if(PlayerController)
        {
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }
}
