// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/MyGameMode.h"

// UE
#include "GameFramework/GameSession.h"
#include "Kismet/Gameplaystatics.h"
// Framework
#include "Framework/MyPlayerController.h"
#include "Framework/MyHUD.h"
#include "Framework/MyPlayer.h"


AMyGameMode::AMyGameMode(const FObjectInitializer& ObjectInitializer)
{
	PlayerControllerClass = AMyPlayerController::StaticClass();
	HUDClass			  = AMyHUD::StaticClass();

	// Later, for using BP's KeyMapping
	const TCHAR* MainCharacter = TEXT("/Game/Blueprints/BP_MyCharacter");
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(MainCharacter); // upcasting
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}

AMyPlayerController* AMyGameMode::Get_PlayerController()
{
	return Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}
