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
- Inside the BP, View Port > Details > Static Mesh: select which static mesh you are going to use to physically represent your player in the world.
- Add a camera and a spring arm component (+AddComponent), attach the camera to the spring arm and adjust its position.
- In BP, View Port > Details > Rendering > disable player hidden in game.

## 1.3: Create our own GameMode BP class

In Unreal, Create a new BP class inheriting from GameMode Base: BP_ShooterGameMode
Inside the BP, Details, Classes, Default Pawn Class, select our main character BP.
In Unreal, Blueprints, GameMode, Select GameMode base class, select our BP_ShooterGameMode class.


# 2: Create Sub-Components: (Gun Component)

- Create a c++ gun component of actor type. 
- Create a BP subclass based on this c++ class: BP_Rifle
- Add components in the Gun actor: Root and Mesh attached to the root
- Select a mesh component for the type of gun

## 2.1: Attach the rifle component to our character component in the scene:

- Spawn rifle component be shown close to our actor mesh
- Hide the gun that is already there: hide the weapon bone: 
- Attach scene component (BP_Rifle) to the socked of the ShooterCharacter skeleton mesh
- Asign the owner for the rifle to be the shooter character

Header file
'''cpp
private: 
	UPROPERTY(EditDefaultsOnly) /*so that no one can edit it at run time*/
	//To Spawn an object in the world from C++ we need to connect the C++ code to the Unreal Blueprint that contains the object's mesh. To do this we use TSubclassOf<>.
		//TSubclassOf<> makes the connection between the C++ code and the blueprint. (Reflection)
	//Use TSubclassOf<> to declare a variable that represents the type of the class of the object we want to spawn in the world
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	//Declare an instance of the class (pointer) Gun for us to store our Gun variable
	AGun* Gun;
'''




# 2: Player Input: 

## 2.1: Settup an axis mapping for movement and an action mapping for firing  

- Unreal > Edit > Project Settings > Input > Bindings > Axis Mapping / Action Mapping
- Click add and create one axis mapping function Move to move forwards and backwards: assign one key for forwards with value +1 and one key for backwards with value -1.
- Click add and create one axis mapping function LookUp: assign one key to look up (mouse Y) with value -1.
- Also create one function Turn for turning left and right and look left and right and assign its respective keys with respective values
- Click add and create one action mapping function for fire and for jump and assign a key to it.

## 2.2: Bind the axis / action mapping to our action callback functions

### 2.2.1: Movement actions

- In the header file, Declare the Move() and Turn() and Fire() funtions.
- Include Speed and Turn Rate variables to fine tune the player's movements.

```cpp
 private:

  void MoveForward(float AxisValue);

  void MoveRight(float AxisValue);

  UPROPERTY(EditAnywhere)
  float RotationRate = 10;
```

- Bind each user input axis or action mapping to its correspondent action callback functions and create the default call back functions for move forward and move right
- Define the action callback functions to Move forward, move right and shoot

In ShooterCharacter.cpp
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



- In unreal > select BP_PawnPlayer > physics > set simulate physics off for both the CapsuleComponent and the BaseMesh
- Make sure you move the capsule and the base mesh a little above the ground so that they don't get stuck in the terrain


# 3: Player Animation


