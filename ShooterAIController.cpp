// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"   


//SERVICES
    //service is software that performs automated tasks, responds to events, or listens for data requests from other software
    //services are more efficient than setting this piece of code on tick because you can setup services to only run when the program executes only a specific part of code
        //such as passing by a specific node in a behavior tree

void AShooterAIController::BeginPlay()
{
    Super::BeginPlay();

    //Run our Behavior tree
        //allows us to set a logic sequence of behavior for our AI
        //Then set it up inside BP_ShooterAIcontroller > Shooter AI Controller > AIBehavior
    if (AIBehavior != nullptr)
    {
        RunBehaviorTree(AIBehavior);

        //Set a new key in BB_EnemyAI > AI Blackboard to store data on players location in begin play
            //Blackboard is like the memory of the AI. Stores variables you input from the game. sets properties that stores information of the world. 
                //and the behavior tree reads the blackboard variables to decide what actions is going to do next
       
        //Get Player pawn
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0); 

        //Now add a sequence of tasks in the decision tree and make the AI move back and forth from its starting point to the players location.

        //Set the AI initial locaiton for the variable in the Blackboard
        GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation()); 
        //Add a new vector variable inside BB_EnemyAI > Blackboard and give it the same name as given inside this function
    }
}

void AShooterAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

//Check if the AI is dead
bool AShooterAIController::IsDead() const
{
    //Check if the pawn is a shooter character type
    AShooterCharacter* ControlledCharacter = Cast<AShooterCharacter>(GetPawn());

    if (ControlledCharacter != nullptr)
    {
        return ControlledCharacter->IsDead();
    }
    
    //if there is not a pawn controlling this character it means that it is dead
    return true;

}
