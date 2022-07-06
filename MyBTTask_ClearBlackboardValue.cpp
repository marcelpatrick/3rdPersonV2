// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBTTask_ClearBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"

//allows us to clear any variable in the blackboard. will clear last know location so that the AI goes back to the initial position if he does not see me
UMyBTTask_ClearBlackboardValue::UMyBTTask_ClearBlackboardValue()
{
    NodeName = TEXT("Clear Blackboard Value");
}

//"Execute task": do this function when the task starts executing
EBTNodeResult::Type UMyBTTask_ClearBlackboardValue::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, /*access our behavior tree component*/
    uint8* NodeMemory /*access the memory pointer that stores info about the node of one particular tree*/
    )
{
    //Call Super because we are overriding the parent class with our custom functionality that we will define here (but we still need to implement all other functionalities from the parente class)
    Super::ExecuteTask(OwnerComp, NodeMemory);

    //Get hold of our blackboard and clear the variable for the selected key
        //Get the key for the variable of this decision tree node
    OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());

    //return that this nodes result so that the decision tree takes the next action based on that, in this case, succeeded
    return EBTNodeResult::Succeeded;
}
