# 3rdPersonV2

# Project Steps:
1. Create Components (Parent Classes)
2. Create Sub-Components (Child Classes: Tank, Tower, Projectile)
3. Player Input (Moving the Actor, Firing)
4. Actions and Events (Hit Events, Health Component, Apply Damage)
5. Game Rules, Game Mode and Game Controller (Game Cycle: Start > Death > Winning, Loosing > End)
6. Special Effects (Sounds, Particles)


# 1: Create Components

## 1.1: Main Character C++ class
Create a new C++ class "ShooterCharacter" inheriting from the Character class that will be the base class for both our player and our enemies.
On File click on Refresh Virtual Studio Code project.

## 1.2: Main Character Blueprints

In Unreal, select the c++ class and click on Create a new blueprint based on "this class", name it "BP_ShooterCharacter".
Inside the BP, View Port > Details > Static Mesh: select which static mesh you are going to use to physically represent your player in the world.
Adjust the camera and the spring arm, camera and projectile spawn point position.
In BP, View Port > Details > Rendering > disable player hidden in game.

## 1.3: Declare Component Variables:
In its header file initialize and expose the variables that will correspond to each of the components that we will use in the world.

```cpp
 
```

## 1.4: Construct Component Objects:
In the cpp file, Construct these components so that they are able to ve visible in our world

```cpp
 
```

## 1.5: Create our own GameMode BP class

In Unreal, Create a new BP class inheriting from GameMode Base: BP_ShooterGameMode
Inside the BP, Details, Classes, Default Pawn Class, select our main character BP.
In Unreal, Blueprints, GameMode, Select GameMode base class, select our BP_ShooterGameMode class.


# 3: Player Input: 


## 3.1: Settup an axis mapping for movement and an action mapping for firing  

- Unreal > Edit > Project Settings > Input > Bindings > Axis Mapping / Action Mapping
- Click add and create one axis mapping function Move to move forwards and backwards: assign one key for forwards with value +1 and one key for backwards with value -1.
- Click add and create one axis mapping function LookUp: assign one key to look up (mouse Y) with value -1.
- Also create one function Turn for turning left and right and look left and right and assign its respective keys with respective values
- Click add and create one action mapping function for fire and for jump and assign a key to it.

## 3.2: Bind the axis / action mapping to our action callback functions

- In the header file, Declare the Move() and Turn() and Fire() funtions.
- Include Speed and Turn Rate variables to fine tune the player's movements.

```cpp
 private:

  void MoveForward(float AxisValue);

  void MoveRight(float AxisValue);

  UPROPERTY(EditAnywhere)
  float RotationRate = 10;
```

- Inside SetupPlayerInputComponent() bind each user input axis or action mapping to its correspondent action callback functions
- Define the action callback functions to Move forward, move right and shoot 

```cpp
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
```

- In unreal > select BP_PawnPlayer > Collision > CapsuleComp > collision presets > set to physicsActor 
- In unreal > select BP_PawnPlayer > physics > set simulate physics off for both the CapsuleComponent and the BaseMesh
- Make sure you move the capsule and the base mesh a little above the ground so that they don't get stuck in the terrain

## 3.3: Fire Function:

- For the player to fire a projectile, we need it to spawn a projectile object from the projectile spawn point. 
- To do this, the Fire() function needs to call a projectile C++ class but this C++ class needs to connect to a blueprint with a mesh so that it can spawn a real object in the world. 
- To make this connection (Reflection) between a C++ class on code and an Unreal Blueprint we need to use a UClass() - Unreal Class - from the type of the object we want to spawn.
- This UClass() will be inside the player C++ code, and will connect to the Projectile blueprint when gets called by the Fire() function.
- To Declare a UClass() we need to use a TSubclassOf<> template function.
- Had we not used UClass() it would only spawn an object based on a raw c++ class which could not contain a static mesh. 

Mouse click > Binding > Fire() > projectile UClass > BP_Projectile > spawn projectile mesh in the world

- In BasePawn.h, Declare the action callback function Fire(). 
- Then use TSubclassOf<> template function to declare a UClass() of type AProjectile.

```cpp
 
```

- In the BP_PawnCowPlayer blueprint > Details > Combat > Projectile Class set BP_Projectile as the ProjectileClass to be spawned by the Player.
- Do the same thing in BP_PawnCowEnemy blueprint.

Define the action callback function Fire() in BasePawn.cpp - because this one will be inherited by both the Player and the Enemy actors.
```cpp
 
``` 


