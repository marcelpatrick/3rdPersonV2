// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Components/InputComponent.h" 
#include "Components/SkeletalMeshComponent.h" 
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"
#include "KillEmAllGameMode.h"
#include "Gun.h"

class AGun; 


// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//get total health
	Health = MaxHealth;

	//Spawn the rifle component from our actor component at run time
	Gun = GetWorld()->SpawnActor<AGun>(GunClass); //Then select the BP_Rifle as the GunClass Mesh in our BP_ShotterCharacter
	
	//Hide the gun that is currently there
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None); //Then, in the Skeleton asset, add a new socket
	
	//Attach the new gun to the actor's hand
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	
	//Set up the gun to have this class as its owner. allows the gun actor to know that the ShooterCharacter is its owner. Useful to assign who is generating damage by shooting which gun. 
	Gun->SetOwner(this);
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;	
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health/MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Set up character movement
		//Setup actions and axis inside project settings
		//Bind movement functions for our character
	PlayerInputComponent->BindAxis(
		TEXT("MoveForward"), /*Axis function name in project settings*/
		this, /*target context*/
		&AShooterCharacter::MoveForward /*Address of the movement function to be called*/
		);

	PlayerInputComponent->BindAxis(
		TEXT("LookUp"), /*Axis function name in project settings*/
		this, /*target context*/
		&APawn::AddControllerPitchInput /*Address of the movement function to be called: already inheriter from Pawn so we don't need to implement it here*/
		);

	PlayerInputComponent->BindAxis(
		TEXT("MoveRight"), /*Axis function name in project settings*/
		this, /*target context*/
		&AShooterCharacter::MoveRight /*Address of the movement function to be called: already inheriter from Pawn so we don't need to implement it here*/
		);

	PlayerInputComponent->BindAxis(
		TEXT("LookRight"), /*Axis function name in project settings*/
		this, /*target context*/
		&APawn::AddControllerYawInput /*Address of the movement function to be called: already inheriter from Pawn so we don't need to implement it here*/
		);

	PlayerInputComponent->BindAction(
		TEXT("Jump"), /*Axis function name in project settings*/
		EInputEvent::IE_Pressed, /*type of input event that will be the trigger for this discrete action*/
		this, /*target context*/
		&ACharacter::Jump /*Address of the movement function to be called: already inherited from ACharacter so we don't need to implement it here*/
		); 

	//PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShooterCharacter::Shoot);

}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//since we are overriding a function, before we should call the parent method to make sure we inherit all the other behaviors we don't want to override
		// The inherited TakeDamage() function returns a damage amount which we will store inside our DamageToApply variable to be able to manipulate it the way we want inside our custom TakeDamage() function
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//if whatever health left is smaller than DamageToApply decreasing health by DamageToApply will give us a negative number. 
		//to avoid this we want to: if DamageToApply is smaller then the Health left, then we decrease by DamageToApply, if the Health left is smaller than DamageToApply, then we decrease by the amount of Health that is left
		//Use whichever is smaller and assign it to DamageToApply
	DamageToApply = FMath::Min(Health, DamageToApply);

	Health -= DamageToApply;

	UE_LOG(LogTemp, Warning, TEXT("Your current Health is %f"), Health);

	if (IsDead())
	{
		//Get a hold of our game mode (the basic game mode that the game will start with - simple shooter) and call the killed pawn
			//And store it in a variable so that we can later switch the game mode type
		ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();

		if (GameMode != nullptr)
		{	
			//Make the game mode aware that this pawn was killed
			GameMode->PawnKilled(this);
		}

		//Stop the character from being controlled - moved.
		DetachFromControllerPendingDestroy();
		//Switch off capsule collision 
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
	

	return DamageToApply;
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	//Add movemen input using the specified movement vector multiplied by the axis value (if positive 1, move forward, if negative 1 backwards)
		//Pass in a vector * a direction value
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::Shoot()
{
	Gun->PullTrigger();
}

