// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class AGun;


UCLASS()
class SIMPLESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Create a function that can be accessed from the character blueprint - BlueprintPure and const function
		//a pure node is a node that doesn't have an execution pin - doesn't have any effect on the thing that it is calling, only on its result
		//every time we call this function it is going to have the same effect on the program and output the same result
		//It doesn't change anything globally. the only impact it has are the outputs that it produces.
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//Include a function for the character to receive damage and override the equivalent virtual function TakeDamage in the Actor parent class
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Shoot();

private:

	void MoveForward(float AxisValue); //Use ctrl shift P > create implementation to automatically define this function in .cpp

	void MoveRight(float AxisValue);

	UPROPERTY(EditAnywhere)
	float RotationRate = 10;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly) /*so that no one can edit it at run time*/
	//To Spawn an object in the world from C++ we need to connect the C++ code to the Unreal Blueprint that contains the object's mesh. To do this we use TSubclassOf<>.
		//TSubclassOf<> makes the connection between the C++ code and the blueprint. (Reflection)
	//Use TSubclassOf<> to declare a variable that represents the type of the class of the object we want to spawn in the world
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	//Declare an instance of the class (pointer) Gun for us to store our Gun variable
	AGun* Gun;

};
