// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
//include "UMG" in the public dependency modules in SimpleShooter.Build


void AShooterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    HUD = CreateWidget(this, HUDClass);
    if (HUD != nullptr)
    {
        HUD->AddToViewport();
    }
    
}

void AShooterPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
    Super::GameHasEnded(EndGameFocus, bIsWinner);

    HUD->RemoveFromViewport();

    if (bIsWinner)
    {
        //print win widget
        UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);

        if (WinScreen != nullptr)
        {
            WinScreen->AddToViewport();
        }
        //In Unreal > BP_ShooterPlayerController > WinScreenClass > select our WBP_WinScreen
    }
    else
    {
        //print lose widget

        //Spawn Widget
        UUserWidget* LoseScreen = CreateWidget(
            this, /*Owning object*/ 
            LoseScreenClass /*Widget class*/
            );

        if (LoseScreen != nullptr)
        {
            //Print our widget to the screen
            LoseScreen->AddToViewport();
        }
        //In Unreal > BP_ShooterPlayerController > LoseScreenClass > select our WBP_LoseScreen
    }
    
    //Set timer to count 5 seconds after we are killed and then restart the level
    GetWorldTimerManager().SetTimer(
        RestartTimer, /* TimerHandle: can be used to pause (and resume) the countdown, query or change the amount of time remaining, or even cancel the timer altogether*/
        this, /* objecto to be called*/ 
        &APlayerController::RestartLevel, /*the address of the function we want to delay with this timer*/
        RestartDelay /*amount of time for that timer to delay*/
        );
}

