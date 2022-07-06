// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Shoot.generated.h"

/**
 * This is a custom task that allows the AI to shoot at me
 */

UCLASS()
class SIMPLESHOOTER_API UBTTask_Shoot : public UBTTaskNode
{
	GENERATED_BODY()

public:

	//Contructor
	UBTTask_Shoot();

protected:

	//Execution
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;
	
};
