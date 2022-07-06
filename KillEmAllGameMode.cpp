// Fill out your copyright notice in the Description page of Project Settings.


#include "KillEmAllGameMode.h"
#include "EngineUtils.h"  
#include "GameFramework/Controller.h"
#include "ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterAIController.h"


void AKillEmAllGameMode::PawnKilled(APawn* PawnKilled)
{
    Super::PawnKilled(PawnKilled);

    UE_LOG(LogTemp, Warning, TEXT("The Pawn was Killed"));
    //In Unreal, change the BP_ShooterGameMode name to BP_KillEmAllGameMode > Open > Class Settings > Change Parent Class to KillEmAllGameMode
        //click on Blueprints in the top bar > GameMode > Select GameMode Base Class > select BP_KillEmAllGameMode

    //GAME OVER: Check if the killed pawn is a player controller (if the player has died) - Lose scenario
    APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());

    if (PlayerController != nullptr)
    {
        EndGame(false);
    }

    //WIN GAME: For loop over all shooterAIs in the world and check if any of them are not dead. then game is not over - return to exist our of this function and stop executing it
    //If we iterate over all AI and they are all dead then end game - we won
    for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
    {
        bool bIsPlayerController = Controller->IsPlayerController();

        if (!Controller->IsDead())
        {
            return; 
        }
    }
    EndGame(true);
}

void AKillEmAllGameMode::EndGame(bool bIsPlayerWinner)
{
    //Iterate over all the controllers / actors in the world so that we can call a function on each of those actors 
        //check if they are of the player controller type (meaning that it is the player, not the AI) and, if it is, check if this player controller is the winner
            //Use an engine helper - EngineUtils and the template function TActorRange
                //Will return a range of actors (like a list) and allows us to iterate in all controllers in the world

    //for (each controller variable : within this range)
    for (AController* Controller : TActorRange<AController>(GetWorld()))
    {
        //if the controller is our player and it is the winner then we won the game
        //if the controller is not our player (is the AI) and it is not the winner, then we also won the game
        //otherwise we lost the game
        bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
        Controller->GameHasEnded(
            Controller->GetPawn(), /*keep the camera focus on the player*/
            bIsWinner /*bool defining if we won or lost*/
            );
    }
        
}
