// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIfSeen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h" 
#include "GameFramework/Pawn.h"

UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen()
{
    NodeName = "Update Player Location If Seen";
}


//update next tick interval if the code passes through this node in the behavior tree
void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (PlayerPawn == nullptr)
    {
        return;
    }

    if(OwnerComp.GetAIOwner() == nullptr)
    { 
        return;
    }

    //If the AI can see the player
    if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn))
    {
        //Then get and update the current player location by the player pawn location
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn); 
    }
    else
    {
        //If AI cannot see the player clear the player location from its memory
        OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
    }
//In our behavior tree, include this service in the Selector node and change the variable BlackboardKey to "LastKnownPlayerLocation"
}

