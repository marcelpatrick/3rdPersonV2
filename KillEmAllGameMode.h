// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleShooterGameModeBase.h"
#include "ShooterAIController.h"
#include "KillEmAllGameMode.generated.h"


/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API AKillEmAllGameMode : public ASimpleShooterGameModeBase
{
	GENERATED_BODY()
	
public:
	//This method is going to override the virtual method in SimpleShooterGameModeBase
	virtual void PawnKilled(APawn* PawnKilled) override;

private:

	void EndGame(bool bIsPlayerWinner);

	AShooterAIController* ShooterAIController;
};
