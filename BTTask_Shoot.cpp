// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Shoot.h"
#include "GameFramework/Controller.h"
#include "AIController.h"  
#include "ShooterCharacter.h"

//Constructor
UBTTask_Shoot::UBTTask_Shoot()
{
    NodeName = TEXT("Shoot");
}

//Define the execute task - that is what this custom task is going to do
EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(
    UBehaviorTreeComponent &OwnerComp, /* Address to access our behavior tree component*/
    uint8 *NodeMemory /* memory pointer that stores info about this tree node*/
    )
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    //Get hold of AI controller and then our pawn - get the Shoot() method from ShooterCharacter
        //go to shooter character and make the Shoot() method public to be accessed from outside of the class

    //if owner component is null return that this custom task has failed
    if (OwnerComp.GetAIOwner() == nullptr)
    {
        EBTNodeResult::Failed;
    }
    
    
    AShooterCharacter* Character = Cast<AShooterCharacter>(OwnerComp.GetAIOwner()->GetPawn());

    if (Character == nullptr)
    {
        return EBTNodeResult::Failed;
    }
    
    Character->Shoot();

    //Will return Succeeded because we only want this task to run once
    return EBTNodeResult::Succeeded;
}
