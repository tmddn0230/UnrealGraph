// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/MyGameModeBase.h"
#include "MyGameMode.generated.h"

/**
 * 
 */

class AMyPlayerController;

UCLASS()
class UNREALGRAPH_API AMyGameMode : public AMyGameModeBase
{
	GENERATED_BODY()
	
private:
	AMyPlayerController* Control_Controller;

public:
	AMyGameMode(const FObjectInitializer& ObjectInitializer);

	AMyPlayerController* Get_PlayerController();

};
