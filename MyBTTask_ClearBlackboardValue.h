// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MyBTTask_ClearBlackboardValue.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UMyBTTask_ClearBlackboardValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	//Constructor of this class
	UMyBTTask_ClearBlackboardValue();

protected:

	//protected because this function is in the default section
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
