# 3rdPersonV2

# Project Steps:

- ITERATION 1: MAIN PLAYER

1. Create Components: Parent Classes
2. Create Sub-Components: Child Classes
3. Player Input: Moving the Actor, Firing
4. Player Animation
5. Actions and Events: Hit Events, Health Component, Apply Damage

- ITERATION 2: ENEMIES AND AI

1. Create Components: Parent Classes
2. Player Input: Moving the Actor, Firing

- ITERATION 3: Game Mode and Effects

1. Game Rules, Game Mode and Game Controller (Game Cycle: Start > Death > Winning, Loosing > End)
2. Special Effects (Sounds, Particles)


# ITERATION 1: MAIN PLAYER


# 1: Create Main Objects

## 1.1: Main Character C++ class
Create a new C++ class "ShooterCharacter" inheriting from the Character class that will be the base class for both our player and our enemies.
On File click on Refresh Virtual Studio Code project.

## 1.2: Main Character Blueprints

In Unreal, select the c++ class and click on Create a new blueprint based on "this class", name it "BP_ShooterCharacter".
- Inside the BP, View Port > Details > Static Mesh: select which static mesh you are going to use to physically represent your player in the world.
- Add a camera and a spring arm component (+AddComponent), attach the camera to the spring arm and adjust its position.
- In BP, View Port > Details > Rendering > disable player hidden in game.

### 1.2.1: Hide default weapon
- Hide the bone attached to the default weapon that already came with the mesh so that we can replace with our own standalone weapon class

ShooterCharacter.cpp
```cpp
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Hide the gun that is currently there
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None); //Then, in the Skeleton asset, add a new socket
}
```

## 1.3: Create our own GameMode BP class

In Unreal, Create a new BP class inheriting from GameMode Base: BP_ShooterGameMode
In Unreal, Blueprints, GameMode, Select GameMode base class, select our BP_ShooterGameMode class.
Include a Player Start component into the level


# 2: Create Sub-Objects

## 2.1: GameMode

- Create a new BP class derived from BP_ShooterCharacter to be our BP_PlayerShooterCharacter, our main player.
- Inside the BP_ShooterGameMode, Details, Classes, Default Pawn Class, select our BP_PlayerShooterCharacter.

## 2.2: Gun

### 2.2.1: Create gun c++ class and blueprint

- Create a c++ gun component of actor type. 
- Create a BP subclass based on this c++ class: BP_Rifle

### 2.2.2: Add components to the Blueprint

- Add components in the Gun actor: Skeletal Mesh attached to the root

Gun.h
```cpp
private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;
```

Gun.cpp
```cpp
#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

}
```

- In Unreal > BP_Rifle > Details > Select a mesh component for the type of gun

### 2.2.3: Create the code inside the character c++ to spawn the gun on Begin Play

- #include Gun.h in both ShooterCharacter.h and ShooterCharacter.cpp
- Create a gun class that will receive our gun blueprint by connecting our c++ code to the Unreal blueprint using TSubclassOf. Make it editable from the character blueprint
- Spawn rifle component be shown close to our actor mesh
- Attach scene component (BP_Rifle) to the socked of the ShooterCharacter skeleton mesh
- Asign the owner for the rifle to be the shooter character

ShooterCharacter.h
```cpp
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gun.h"
#include "ShooterCharacter.generated.h"


private: 
	UPROPERTY(EditDefaultsOnly) /*so that no one can edit it at run time*/
	//To Spawn an object in the world from C++ we need to connect the C++ code to the Unreal Blueprint that contains the object's mesh. To do this we use TSubclassOf<>. TSubclassOf<> makes the connection between the C++ code and the blueprint. (Reflection)
	//Use TSubclassOf<> to declare a variable that represents the type of the class of the object we want to spawn in the world
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	//Declare an instance of the class (pointer) Gun for us to store our Gun variable
	AGun* Gun;
```

ShooterCharacter.cpp
```cpp
#include "Engine/World.h" 
#include "Components/SkeletalMeshComponent.h" 
#include "Gun.h"
#include "ShooterCharacter.h"


void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Spawn the rifle component from our actor component at run time
	Gun = GetWorld()->SpawnActor<AGun>(GunClass); //Then select the BP_Rifle as the GunClass Mesh in our BP_ShotterCharactert
	
	//Attach the new gun to the actor's hand
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	
	//Set up the gun to have this class as its owner. allows the gun actor to know that the ShooterCharacter is its owner. Useful to assign who is generating damage by shooting which gun. 
	Gun->SetOwner(this);
	
	//Then go to Unreal, set the rifle in the right position in the world copy the location coordinates, then open BP_Rifle and paste the coordinates in the mesh component
}
```

### 2.2.3: Link our gun blueprint to our c++ gun class inside the character blueprint

- In Unreal > BP_ShooterCharacter > Details > ShooterCharacter > Gun Class > select BP_Rifle


# 3: Player Input: 

## 3.1: Settup an axis mapping for movement and an action mapping for firing  

- Unreal > Edit > Project Settings > Input > Bindings > Axis Mapping / Action Mapping
- Click add and create one axis mapping function Move to move forwards and backwards: assign one key for forwards with value +1 and one key for backwards with value -1.
- Click add and create one axis mapping function LookUp: assign one key to look up (mouse Y) with value -1.
- Also create one function Turn for turning left and right and look left and right and assign its respective keys with respective values
- Click add and create one action mapping function for fire and for jump and assign a key to it.

## 3.2: Bind the axis / action mapping to our action callback functions

### 3.2.1: Movement actions
 
 
#### 3.2.1.1: Declare the callback functions

- Declare the Move() and Turn() funtions.
- Include Rotation Rate variables to fine tune the player's movements.

ShooterCharacter.h
```cpp
 private:

  void MoveForward(float AxisValue);

  void MoveRight(float AxisValue);

  UPROPERTY(EditAnywhere)
  float RotationRate = 10;
```

#### 3.2.1.2: Define callback functions

```cpp
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
```

#### 3.2.1.3: Bind axis mapping to callback functions

- Into the pre existing SetupPlayerInputComponent function, bind each movement function to its action mapping.
- Bind each user input axis or action mapping to its correspondent action callback functions and create the default call back functions for move forward and move right
- Define the action callback functions to Move forward, move right

- In unreal > BP_ShooterCharacter > set simulate physics off for both the CapsuleComponent and the BaseMesh
- Make sure you move the capsule and the base mesh a little above the ground so that they don't get stuck in the terrain
- In Unreal > BP_ShooterCharacter > select the camera component > details > camera options > select use pawn control rotation 

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
}
```

### 3.2.2: Fire / Shoot actions

#### 3.2.2.1: Create the pull trigger function inside Gun c++ 

Gun.h
```cpp
public:	
	void PullTrigger();
```

Gun.cpp
```cpp
void AGun::PullTrigger()
{
	UE_Log(LogTemp, Warning, TEXT("You have pulled the trigger"));
}
```

#### 3.2.2.2: Declare callback functions

- Declare our Shoot() callback function in ShooterCharacter.h 

ShooterCharacter.h
```cpp
public:
	void Shoot();
```

#### 3.2.2.3: Define callback functions

- Define Shoot() callback function
- Call our PullTrigger function from within Shoot() callback function inside the ShooterCharacter c++

ShooterCharacter.cpp
```cpp
void AShooterCharacter::Shoot()
{
	Gun->PullTrigger();
}
```

#### 3.2.2.4: Bind axis mappings to callback functions

- Bind the Shoot action mapping to our Shoot() call back function
```cpp
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShooterCharacter::Shoot);
}
```

# 4: Player Animation 

- Create an Animation Blueprint: In Unreal, Add new > Animation > Animation Blueprint > select target skeleton > select that of our character (wraith) > call it ABP_ShooterCharacter
- in BP_ShooterCharacter > Details > Animation > Anim Class > select our own custom animation blueprint: ABP_ShooterCharacter

## 4.1: Set the blueprint logic for character movement

in ABP_ShooterCharacter > Event Graph:

Use Event Blueprint Update Animation to get update information on the pawn movements
Use Try Get Pawn Owner to get the pawn object (BP_ShooterCharacter) and get info from the pawn about its movement.

- Set the character's aim: 
	- EventBlueprintUpdateAnimation / TryGetPawnOwner > ?IsValid > Sequence > (create an SetAimPitch float) > link to execution pin for SetAimPitch
	- TryGetPawnOwner > GetControlRotation / GetActorRotation > Delta (Rotator): return valueY > SetAimPitch
		-  Delta (Rotator): control rotation is global. we need to find the delta distance btw the actor's rotation and the global control rotation

- Set Speed: 
	- EventBlueprintUpdateAnimation > ?IsValid > Sequence > (create an SetSpeed float) > link to execution pin for SetSpeed
	- TryGetPawnOwner > GetVelocity > VectorLength > SetSpeed

- Set Angle: 
	- EventBlueprintUpdateAnimation > ?IsValid > Sequence > (create an SetAngle float) > link to execution pin for Angle
	- TryGetPawnOwner > GetActorTransform / GetVelocity > InverseTransformDirection > RotationFromXVector > SetAngle
		- GetActorTransform: transform to rotate the pawn by indicating where the pawn is in the world
		- InverseTransformDirection: velocity is in global space and we should convert it to local space in order to get the angle (direction) to which the player should be going
		- RotationFromXVector: set the yawn to know how far it is turning to the right or left. (Right click on Return Value, select "split struct pin" to get only the return for the yaw.

- Set IsAirBorn: check whether the character is jumping or falling
	- TryGetPawnOwner / sequence execution pin > CastToCharacter > (create a Is AirBorn bool) > link execution pin to Set IsAirBorne
	- TryGetPawnOwner > CastToCharacter (object) > pull from As Character > IsFalling > Set IsAirBorne

![image](https://user-images.githubusercontent.com/12215115/177312724-86f06761-4cda-4af9-b781-72260ff46eca.png)

## 4.2: Animation Graph and State Machines

- Open ABP_ShooterCharacter > AnimGraph

- Add a new state machine "Death" > link it to the output pose
- ![image](https://user-images.githubusercontent.com/12215115/173813210-4e8d38aa-505d-484f-8a4f-e1ce7b580b1a.png)

- Inside the Death state add new states: Entry > Alive > Dead > Alive 
- ![image](https://user-images.githubusercontent.com/12215115/176882960-988cd267-8111-4c3d-93f9-8e8263208c52.png)
	- Create a bool "IsDead"
	- In Alive to Dead: Get IsDead and link to result
	- In Dead to Alive: Get IsDead > NOT > link to result
	- Inside Dead, plug a death animation to result
	- In Details > Anim Preview Editor > set IsDead manually to true or false and click "apply" just for testing purposes

- In the Alive node 
	- Include a new node to be our idle default pose: 
		- From Asset Browser, pull in "Idle_AO_Combat" > Output animation pose
	- Adjust the pitch for our default pose in the AnimGraph according to our aim, so that the character can aim up, down:
		- Idle_AO_Combat > right click on Pitch > promote to variable > "AimPitch"
		- ![image](https://user-images.githubusercontent.com/12215115/173802505-381098c3-37d6-408b-8866-3417a4a43213.png)
	- Add locomotion capabilities to our idle pose: Create a Locomotion state machine > Idle_IO_Combat base pose
		- Inside Locomotion: add new states and link them with transitions. Edit the states based on float var and transitions based on bool variables. These var need to be set in two places: In the EventGraph so that an event can set their value and in the AnimGraph to tell the state machine the direction of an animation or when to enter a transition: Entry > Grounded > Jumping > Airborne > Landing > Grounded 
		- ![image](https://user-images.githubusercontent.com/12215115/173815866-dba7a701-a70a-4b64-82c4-7c6bc5a0a6e7.png)
			- In Grounded: 
			- Create a new Blend Space animation to make a soft transition between different animations 	
				- In Unreal > Add New > Animation > Blend Space > select Wraith skeleton > "BS_Locomotion"
				- Inside BS_Locomotion, go in details > asset browser > and drag in the animations to each movement node: 
				- ![image](https://user-images.githubusercontent.com/12215115/176886649-76f35080-51e2-457c-a551-9a4b04a5f99b.png)
			- Get Angle var / Get Speed var > BS_Locomotion > Output animation pose
			- ![image](https://user-images.githubusercontent.com/12215115/173811140-ca45782f-8f6a-40fb-b3cb-72311d7ae217.png)
			- In Grounded to Jumping: add variable > bool "IsAirborne" > get IsAirborne > Result can enter transition
			- ![image](https://user-images.githubusercontent.com/12215115/173811212-86bb6ccd-abbc-4b3c-a99e-7d0757d6f448.png)
			- In Jumping: add Jump_Start_Combat > Output animation pose
			- ![image](https://user-images.githubusercontent.com/12215115/173811374-8d965aad-65c2-4e9e-a481-79002abc2880.png)
			- In Jumping to Airborne: add variable > bool "IsAirborne" > get IsAirborne > Result can enter transition
![image](https://user-images.githubusercontent.com/12215115/176880225-6544ba09-c780-4234-9f1d-87cca4d839bf.png)
			- In Airborne: Output Animation pose > pull off result pin > Play Jump_Apex_Combat > Details > Settings > uncheck Loop Animation
			- ![image](https://user-images.githubusercontent.com/12215115/173572214-358beef7-0414-4ea2-8428-2745513487e0.png)
			- In Airborne to Landing: IsAirborne bool > NOT > Result
			- ![image](https://user-images.githubusercontent.com/12215115/173573650-899a2e14-38e8-423a-b1e5-49e92616f2cf.png)
			- In Landing: Add Jump_Land_Combat > Output animation pose
			- ![image](https://user-images.githubusercontent.com/12215115/173811486-c8940066-1f19-4f57-b9b8-8121577ba4c5.png)
			- In Landing to grounded: Get IsAirborne bool > NOT > Result: Can Enter the Transition
			- ![image](https://user-images.githubusercontent.com/12215115/173811802-70ba0895-f3f3-4392-92bc-a6b72bd55c72.png)
			
- In Alive to Dead: Get IsDead bool > Result
- ![image](https://user-images.githubusercontent.com/12215115/173814563-3e1e0eeb-3ba3-497d-89df-c2b7fa0021c0.png)

- In the Dead node: Add Play Death_Forward animation > Output animation pose
- ![image](https://user-images.githubusercontent.com/12215115/173813490-235e6886-4523-4567-971d-e1380f1d24a9.png)
	
- In Dead to Alive: Get IsDead bool > NOT > Result
- ![image](https://user-images.githubusercontent.com/12215115/173814767-e11acd10-f220-4da8-bf52-36e87d7af2cf.png)
	

# 5. Actions and Events: Hit Events, Health Component, Apply Damage


## 5.1: Set Gun Line tracing

- In order to hit and damange something we need to define which is the line trace that the bullet will do after it is shoot so that we can define what targets were in reach and which got actually hit. 
- So in Gun c++ :
	- Set player view point
	- Set line trace by channel

Gun.h
```cpp
private:
	bool GunTrace(FHitResult& Hit, FVector& ShotDirection);

	AController* GetOwnerController() const;
```

Gun.cpp
```cpp

bool AGun::GunTrace(FHitResult& Hit /*a hit result out param (will be modified by this function), with an & because it is a reference to this variable's address*/,
				 FVector& ShotDirection /*a shot direction out param (will be modified by this function)*/)
{
	//Call the GetOwnerController() function to get the controller and assign it to a AController variable
	AController* OwnerController = GetOwnerController(); 

	//Get Player's location and rotation
	FVector Location;
	FRotator Rotation;

		//Use GetPlayerViewPoint to fill in the Location and Rotation variables as out paramters
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	//Create a line trace for the bullet

		//Declare and define variable with the shot starting point (it is going to come from where the player stands)
		FVector Start = Location;

		//Declare and define a variable with the shot direction (it will point to where the player is pointing)
			//Rotation.Vector() gets a vector that points in the direction to which the rotation is pointing of size = 1
		ShotDirection = Rotation.Vector();

		//Declare and define a variable with the shot end point
		FVector End = Start + ShotDirection * MaxRange;

		//Define actors that should be ignored: this and GetOwner() so that the actor doesn't shoot itself
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwner());

	//Return LineTraceBySingleChannel passing Hit result, Start point, End point, the collision channel, and settings params
		//Set our custom trace channel in Project Settings > Collision > Trace Channels > define presets to define interaction with each type of objects
		//Fetch the enum of the collision channel for our bullet in Project Folder > Config > Default Engine > search for the name of the custom collision channel we crated, "Bullet" and see which channel was asigned to it
	return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, Params);

}

AController* AGun::GetOwnerController() const
{
	//Get the viewpoint of the player. need to get the player controller from the gun owner
		//Gun owner is an actor so we need to cast it to APawn (because our shooter character inherits from APawn)
	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (OwnerPawn == nullptr) return nullptr;

	//Get the controller for the gun owner
	return OwnerPawn->GetController();
}

```

- in Gun.cpp, Call the GunTrace() function from inside the PullTrigger() function and asign the result to a bool

```cpp
void AGun::PullTrigger()
{
	FHitResult Hit;
	FVector ShotDirection;
	bool bSuccess = GunTrace(Hit, ShotDirection);
} 
```

## 5.2: Send Damage to the player

- Include another BP_ShooterCharacter into the world. Unreal will automatically make this second actor AI controlled 
- Inside Gun c++:
	- Get a DamageEvent 
	- Get a hold of the actor we will damage
	- Call the custom TakeDamage() function that Gun is inheriting from the parent actor class

Gun.h
```cpp
	UPROPERTY(EditAnywhere)
	float DamageAmount = 100;
```

Gun.cpp
```cpp
void AGun::PullTrigger()
{
	UE_LOG(LogTemp, Warning, TEXT("vc puxou o gatilho"));

	FHitResult Hit;
	FVector ShotDirection;
	bool bSuccess = GunTrace(Hit, ShotDirection); //Hit and ShotDirections are out params - will have their values assigned by this function

	//If the shot hit something
	if (bSuccess)
	{
		//Get the actor that got hit
		AActor* HitActor = Hit.GetActor();

		//And if this something we hit was an actor
		if (HitActor != nullptr)
		{
			//Fill in a DamageEvent struct
				//FPointDamageEvent is a category of damage event that is applied to a specific point
				//It is also of type "struct" - struct is like an array that can take different types of variables at the same time
				//by doing FPointDamageEvent DamageEvent(values) we are initializing this struct of type FPointDamageEvent and already filling it the values for each of its correspondent variables
				//Then we will pass this struct filled in with values in our TakeDamage function below
			FPointDamageEvent DamageEvent(DamageAmount, Hit, ShotDirection, nullptr);

			//Get the Damage Instigator
			AController* Instigator = Cast<AController>(HitActor->GetInstigator());

			//Apply damage to the actor that got hit
			HitActor->TakeDamage(DamageAmount, DamageEvent, Instigator, this);

			UE_LOG(LogTemp, Warning, TEXT("vc atingiu o ator: %s"), HitActor);
		}
	}
}
```

## 5.3: Receive Damage to the player

- Create our own custom TakeDamage() function overriding the custom TakeDamage() function from the Actor class so that we can receive this damage in our custom actor. Store damage inside the actor and not in its own component. 
	- Set initial health in begin play
	- Define our custom TakeDamage()

ShooterCharacter.h
```cpp
public:
	//Include a function for the character to receive damage and override the equivalent virtual function TakeDamage in the Actor parent class
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;
```

ShooterCharacter.cpp
```cpp
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//get total health
	Health = MaxHealth;
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
	
	return DamageToApply;
}
```

## 5.4: Death / Health Components

### 5.4.1: Death Function and Health in c++

ShooterCharecter.h
```cpp
#include "Components/CapsuleComponent.h" 

public:	
	// Create a function that can be accessed from the character blueprint - BlueprintPure and const function
		//a pure node is a node that doesn't have an execution pin - doesn't have any effect on the thing that it is calling, only on its result
		//every time we call this function it is going to have the same effect on the program and output the same result
		//It doesn't change anything globally. the only impact it has are the outputs that it produces.
	UFUNCTION(BlueprintPure)
	bool IsDead() const;
```

ShooterCharacter.cpp
```cpp
bool AShooterCharacter::IsDead() const
{
	return Health <= 0;	
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead())
	{

		//Stop the character from being controlled - moved.
		DetachFromControllerPendingDestroy();
		//Switch off capsule collision 
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
```


### 5.4.2: Hook up Death and Health function to our animation BP

- In Unreal, ABP_ShooterCharacter, Event Graph, create a IsDead bool and hook it up to our animation graph
- TryGetPawnOwner > CastToShooterCharacter (get pawn owner and cast it to a ShooterCharacter) > IsDead (call function from ShooterCharacter.cpp) > Set IsDead 

![image](https://user-images.githubusercontent.com/12215115/170482459-d6fbb0c0-76bd-4524-b5af-62cf1afc59f6.png)


# ITERATION 2: ENEMIES AND AI


# 1: Create Components

- Include a new BP_ShooterCharacter in the world to be our enemy
- Create a new c++ inheriting from AI Controller class, name ShooterAIController. 
- Create e new BP to be child from the ShooterAIController class: BP_ShooterAIController
- In Unreal, open BP_ShooterCharacter > details > Pawn > AIController Class > choose BP_ShooterAIController

# 2: Player Input: AI Aiming, AI firing, AI movement (with Behavior Trees)

Hook up the blueprints: 
- Animation: BP_ShooterCharacter > hooked to Anim Class: ABP_ShooterCharacter > hooked to Mesh: Wraith
- AI: BP_ShooterCharacter > hooked to AI Controller Class: BP_ShooterAIController > hooked to behavior tree AIBehavior: BT_EnemyAI

- Set AI Path finder: create a mesh to tell the AI where in the world it can navigate
	- In Unreal: Modes > search for Nav Mesh Bounds Volume > drag it into the world and put it through the floor
	- In Unreal, in the world screen > Show > check Navigation
	- Increase nav mesh X and Y sizes to encompass the entire level

## 2.1: Set a behavior tree to control character movement
	- Add New > Artificial Intelligence > Behavior tree and Blackboard : "BT_EnemyAI", "BB_EnemyAI"
	- Declare the AIController component in c++
	- Get hold of the pawn actor in c++

ShooterAIController.h
```cpp
UCLASS()
class SIMPLESHOOTER_API AShooterAIController : public AAIController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

private:
	//Declare a behavior tree variable
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
}
```

ShooterAIController.cpp
```cpp
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
    }
}
```

- Hook up our behavior tree to BP_ShooterAIController: in Unreal > BP_ShooterAIController > Details > AIBehavior > select BT_EnemyAI

## 2.2: Set player location at begin play defining a vector variable

- Define the vector var in c++

ShooterAIController.cpp
```cpp
void AShooterAIController::BeginPlay()
{
        //Set the AI initial locaiton for the variable in the Blackboard
        GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation()); 
        //Add a new vector variable inside BB_EnemyAI > Blackboard and give it the same name as given inside this function
    }
}
```

### 2.3: Define the vector variables on Blackboard

- Create a vector variable for PlayerLocation: Blackboard details > key > key type > Vector > rename : PlayerLocation
- Create a vector variable for StartLocation
- Create a vector variable called LastKnownPlayerLocation
- Create an object variable with base class "Actor" called SelfActor
- Create an object variable with base class "Actor" called Player > base class "actor"

### 2.4: Build the behavior trees nodes

- Logic: If AI sees player, run towards him. If it doesn't, investigate moving to the player's last know location

#### 2.4.1: Update player location if seen

- If AI sees player, updated player location in the AI memory
- In BT_EnemyAI, Behavior Tree section, After Root, include a SELECTOR to shift between behavior nodes
	- SELECTOR runs all behaviors in the tree until the first one succeeds. Moves through the ones that fail. Stops when succeeds. Performs only one task (the first that is viable)
	- SEQUENCE runs all behaviors until one of the fails. Runs all behaviors that succeed. Stops when fails. Performs all tasks that are viable.
 
 - Include AIModule and GameTasks as Dependency Modules inside the project build:
 
 in [ProjectName].Build.cs:
 ```cpp
 PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "GameplayTasks" });
 ```

 - include a SERVICE to Update the Player Location in the AI location memory (PlayerLocation) var only if the player is seen by the AI
 	- Create a new BTService class: In Unreal > Add New > New C++ class > show all classes > BTService_BlackboardBase: call it BTService_PlayerLocationIfSeen
	- BTService_BlackboardBase is a custom service that allows us to refer to the variables or keys we included in the Blackboard

##### 2.4.1.1: Implement the Constructor and give this Behavior Tree node a name

 BTService_PlayerLocationIfSeen.h
 ```cpp
 class SIMPLESHOOTER_API UBTService_PlayerLocationIfSeen : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	//Constructor
	UBTService_PlayerLocationIfSeen();
} 
 ```
 
 BTService_PlayerLocationIfSeen.cpp
 ```cpp
 UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen()
{
    NodeName = "Update Player Location If Seen";
}
```

##### 2.4.1.2: Implement the Tick Node Function
 
- Get PlayerPawn
- Use the OwnerComp class to: 
	- IF AI line of sight reaches PlayerPawn
	- Update the selected blackboard key value with PlayerPawn
	- Otherwise clear the value of the selected blackboard key
	- If PlayerPawn or AIOwner are nullptr, return

 BTService_PlayerLocationIfSeen.h
 ```cpp
 class SIMPLESHOOTER_API UBTService_PlayerLocationIfSeen : public UBTService_BlackboardBase
{
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
 ```
 
  BTService_PlayerLocationIfSeen.cpp
 ```cpp
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
        //Then get and update the current player location by following it's actor object (pawn)
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn); 
    }
    else
    {
        //If AI cannot see the player clear the player location from it's memory
        OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
    }
//In our behavior tree, include this service in the Selector node and change the variable BlackboardKey to "LastKnownPlayerLocation"
}
 ```
 
 - In BT_EnemyAI > BehaviorTree: right click on the SELECTOR > add service > select our custom service "Player Location If Seen"
 - In BT_EnemyAI > BehaviorTree: details > blackboard key > select: LastKnownPlayerLocation

![image](https://user-images.githubusercontent.com/12215115/173561306-09784283-f638-4b78-9181-e29ab0a39865.png)

#### 2.4.2: Can See Player ? > Chase Player
 
 - After the selector > Add a new sequence called Chase 
 - Right click on the Chase sequence > Add a decorator of type Blackboard > call it "Can See Player?" > in details > Blackboard > key query = is set > blackboard key = Player
 	- Blackboard condition node: only executes the sequence based on a condition related to a blackboard variable: if PlayerLocation is set
 - Add a new Move To node after Chase > in details > blackboard > blackboard key = Player
 - in Details > Blackboard > check Observer Blackboard value
 
 ![image](https://user-images.githubusercontent.com/12215115/173561461-008691dd-545f-4959-800e-b431c75b2402.png)

 #### 2.4.3: Has Investigated? > Investigate
 
 - Go to where the player was seen last. if LastKnowPlayer location is set (if AI saw where player was last) then walk there to investigate.
 
  - After the selector > Add a new sequence called Investigate 
  - Add a new Move To node after Investigate > in details > blackboard > blackboard key = LastKnownPlayerLocation
  - Click on the "Can See Player" Blackboard decorator > In details > flow control > observer aborts > select both : it aborts both the nodes in the selector (Chase and Investigate) in case something fails.
  -  Right click on the Investigate sequence > Add a decorator of type Blackboard > call it "has Investigated?" > in details > Blackboard > key query = is set > blackboard key = StartLocation
 
 ![image](https://user-images.githubusercontent.com/12215115/173561584-e8da652a-1ccc-466f-be6e-14eda9863b3f.png)

  #### 2.4.3.1: Custom Tasks: Clear Blackboard Value
  
 - Forget about where the player was last and move back to the initial position. Clear LastKnowPlayer location from memory.
  
 - Create a new BTTask class: In Unreal > Add New > New C++ class > show all classes > BTTask_BlackboardBase: call it BTTask_ClearBlackboardValue
 	- BTTask_BlackboardBase is a custom Task that allows us to refer to the variables or keys we included in the Blackboard
 - In Visual Studio, open SimpleShooter.Build.cs, inside the PublicDependencyModuleNames.AddRange function, add "GameplayTasks" to the list and compile

- Create a public constructor
- Implement the Execute Task which we are going to use to access memory variables on Blackboard from our code and then clear the one related to LastKnowPlayerLocation
	- Tasks are types of behavior we can assign to a specific node inside a behavior tree. There are 4 types of tasks: ExecuteTask, AbortTask, TickTask, OnMessage

BTTask_ClearBlackboardValue.h
```cpp
public:
	//Constructor of this class
	UMyBTTask_ClearBlackboardValue();
	
protected:
	//protected because this function is in the default section
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
```

- define the name of this node
BTTask_ClearBlackboardValue.cpp
```cpp
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
```

- in Unreal > BT_EnemyAI > pull a new node after Investigate > select our custom task Clear Blackboard Value > in details > Blackboard > Blackboard key > select LastKnownPlayerLocation
- pull another new node after Investigate > Wait

![image](https://user-images.githubusercontent.com/12215115/173561751-aa5a4718-175d-454b-b823-788bbafa8667.png)

 ### 2.4.4: Move to StartLocation
 
 - in Unreal > in BT_EnemyAI > After the SELECTOR node > pull a new MoveTo node > select StartLocation as a variable / Blackboard Key 

![image](https://user-images.githubusercontent.com/12215115/173561901-6d567be1-82f5-46dc-abcd-96330d11e22c.png)

 ### 2.4.5: Shoot
  
  - In Unreal > Add New > New C++ class > select BTTaskNode > rename to BTTask_Shoot
  - Implement the BTTask_Shooter constructor
  - Implement the ExecuteTask to get hold of the AI Controller and the AI Pawn, pass it in a Character variable, and them call the Shoot() method from this variable
  - Inside ShooterCharacter.h, make the Shoot() method public to allow it to be called from outside of this class

in BTTask_Shoot.h
```cpp
public:

	//Contructor
	UBTTask_Shoot();

protected:

	//Execution
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory) override;
```

in BTTask_Shoot.cpp
```cpp
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
```

- In Unreal > BT_EnemyAI > after Chase > pull a new node > SEQUENCE > add a decorator > Loop > details > check infinite loop
- In Unreal > BT_EnemyAI > after SEQUENCE > pull a new node > our custom Shoot function
- In Unreal > BT_EnemyAI > after SEQUENCE > pull a new node > wait one second

#### 2.4.5.1: Aim at the player to shoot, while chasing
- Use Services: Services allow us to run a piece of code everytime a specific node in the Behavior tree is active
- It is better to update it on tick from the behavior tree than on tick from code because tick from code runs every frame while in the tick from behavior tree we can select the frequency in seconds you want to run it and also it only runs if the particular node is active

##### 2.4.5.1.1: Built in Service
- Set the Default Focus
- In Unreal > BT_EnemyAI > Create a player variable in Blackboard
- In Unreal > BT_EnemyAI > in the Chase node > right click and select add Service > Default Focus > in Details > Blackboard key > select the Player variable
	
![image](https://user-images.githubusercontent.com/12215115/173563080-e1b2bd95-5e4c-4cda-854a-0da020490cc1.png)

##### 2.4.5.1.2: Custom Services

###### 2.4.5.1.2.1: Update LastKnownPlayerLocation
- Create Services to update LastKnownPlayerLocation from the behavior tree while the AI is chasing the player (while the Chase node is active)

####### 2.4.5.1.2.1.1: Create a C++ class

- class to customize the actions of this service in code
- In Unreal > Add New > New C++ class > show all classes > BTService_BlackboardBase > BTService_PlayerLocation

- Add a constructor function
- Add a tick function

BTService_PlayerLocation.h
```cpp
public:
	//Contructor - which is the name of the class
	UBTService_PlayerLocation();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
```

- Define the constructor function and give the node a name
- Define the tick function: For every tick, get the updated player location
	- In cpp > Get Player Pawn > Get blackboard component > Get Selected Blackboard key > get actor location
	
BTService_PlayerLocation.cpp
```cpp
UBTService_PlayerLocation::UBTService_PlayerLocation()
{
    NodeName = "Update Player Location";
}

//update next tick interval if the code passes through this node in the behavior tree
void UBTService_PlayerLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (PlayerPawn == nullptr)
    {
        return;
    }

    OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerPawn->GetActorLocation());
}
```

####### 2.4.5.1.2.1.2: Set the class in our behavior tree

- In our Behavior tree, include this custom service in the Chase node > right click on the Chase node > add service > Player Location > the service will be named "Update Player Location" > change its BlackboardKey variable to "LastKnownPlayerLocation"

###### 2.4.5.1.2.2: Update PlayerLocation 
- Service to update PlayerLocation if we have line of sight and clear it if we don't

####### 2.4.5.1.2.2.1: Create a C++ class

- class to customize the actions of this service in code
- In Unreal > Add New > New C++ class > show all classes > BTService_BlackboardBase > "BTService_PlayerLocationIfSeen"

- Add a constructor function
- Add a tick function

BTService_PlayerLocationIfSeen.h
```cpp
public:
	//Constructor
	UBTService_PlayerLocationIfSeen();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
```

- Define the constructor function and give the node a name
- Define the tick function: For every tick, get the updated player location
	- If the AI can see the player > get and update the current player location by the player pawn location
	- If AI cannot see player > clear the player location from its memory

BTService_PlayerLocationIfSeen.cpp
```cpp
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
```

####### 2.4.5.1.2.2.2: Set the class in our behavior tree

- In our Behavior tree, include this custom service in the SELECTOR node > right click on the node > add service > Player Location If Seen > the service will be named "Update Player Location If Seen" > change its BlackboardKey variable to "Player"


# ITERATION 3: Game Mode and Effects


# 1: Game Mode
- Will use our custom KillEmAllGameMode that will be derived from and implement the default main game mode, SimpleShooterGameMode

## 1.1: Pawn Killed function
- Define a function for when the actors are killed: Use our custom PawnKilled() method in KillEmAllGameMode which will override the virtual PawnKilled() method in the parent SimpleShooterGameModeBase.

SimpleShooterGameModeBase.h
```cpp
public:
	virtual void PawnKilled(APawn* PawnKilled);
```

SimpleShooterGameMode.cpp
- just create an empty implementation so that our child custom game mode can inherit from this parent game mode
```cpp
void ASimpleShooterGameModeBase::PawnKilled(APawn* PawnKilled)
{
}
```

- Create a subclass of SimpleShooterGameModeBase: In Unreal > add new > new C++ class > show all classes > SimpleShooterGameModeBase > "KillEmAllGameMode"
- in KillEmAllGameMode > details > default pawn class > select our main player: BP_PlayerShooterCharacter
- If actor is dead, get hold of game mode and call the pawn killed.

ShooterCharacter.cpp
```cpp
float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
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
	}
```

- Override the KillEmAllGameMode so that we can log out when pawn is killed and set this as the default game mode

KillEmAllGameMode.h
```cpp
public:
	//This method is going to override the virtual method in SimpleShooterGameModeBase
	virtual void PawnKilled(APawn* PawnKilled) override;
```

KillEmAllGameMode.cpp
```cpp
void AKillEmAllGameMode::PawnKilled(APawn* PawnKilled)
{
    Super::PawnKilled(PawnKilled);

    UE_LOG(LogTemp, Warning, TEXT("The Pawn was Killed"));   
}
```

- In Unreal > change the BP_ShooterGameMode name to BP_KillEmAllGameMode > Open > Class Settings > Change Parent Class to KillEmAllGameMode 
- In Unreal > BP_KillEmAllGameMode > Class Defaults > Change Default Pawn Class to KillEmAllGameMode 
- In Unreal > click on Blueprints in the top bar > GameMode > Select GameMode Base Class > select BP_KillEmAllGameMode

## 1.2: Game Over

- Implement a player controller to restart the level after the player has died

- In Unreal > add new > new C++ class > PlayerController > "ShooterPlayerController"
- In Unreal > create a new blueprint class based on our C++ ShooterPlayerController > "BP_ShooterPlayerController"
- In Unreal > In BP_KillEmAllGameMode > Details > Classes > PlayerControllerClass > change to BP_ShooterPlayerController

- PawnKilled() WHO GOT KILLED? > EndGame() WIN OR LOOSE? > GameHasEnded() WHAT TO DO WHEN GAME HAS ENDED?

### 1.2.1: GameHasEnded(): WHAT TO DO WHEN GAME HAS ENDED?

- Implement our GameHasEnded() in our player controller class, to be called from EndGame(), and show win or loose widgets into our viewport and restart the game

ShooterPlayerController.h
```cpp
public: 
	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

private:
	UPROPERTY(EditAnywhere)
	float RestartDelay = 5.f;

	FTimerHandle RestartTimer;

	UPROPERTY(EditAnywhere)
	UUserWidget* HUD;
```

ShooterPlayerController.cpp
```cpp
void AShooterPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
    Super::GameHasEnded(EndGameFocus, bIsWinner);
    
    //Set timer to count 5 seconds after we are killed and then restart the level
    GetWorldTimerManager().SetTimer(
        RestartTimer, /* TimerHandle: can be used to pause (and resume) the countdown, query or change the amount of time remaining, or even cancel the timer altogether*/
        this, /* objecto to be called*/ 
        &APlayerController::RestartLevel, /*the address of the function we want to delay with this timer*/
        RestartDelay /*amount of time for that timer to delay*/
        );
}
```

### 1.2.2: EndGame(): WIN OR LOSE?

- Implement EndGame() to end the game define win and loose conditions. Win (all enemy pawns were killed), loose (player pawn was killed). Then call GameHasEnded() passing the result of this function as a parameter.

KillEmAllGameMode.h
```cpp	
private:
	void EndGame(bool bIsPlayerWinner);
	AShooterAIController* ShooterAIController;
```

- Since we call GameHasEnded() from within a actor controller, we have to know which specific controller won. If the player, the player controller has to call GameHasEnded() - win condition. If the AI, the AI controller has to call GameHasEnded() - lose condition.

KillEmAllGameMode.cpp
```cpp
void AKillEmAllGameMode::EndGame(bool bIsPlayerWinner)
{
    //Iterate over all the controllers / actors in the world so that we can call a function on each of those actors 
        //check if they are of the player controller type (meaning that it is the player, not the AI) and, if it is, check if this player controller is the winner
            //Use an engine helper - EngineUtils and the template function TActorRange
                //Will return a range of actors (like a list) and allows us to iterate in all controllers in the world

    //for (each controller variable : within this range)
    for (AController* Controller : TActorRange<AController>(GetWorld()))
    {
	// if the controller is the player controller (If the player is the one calling EndGame()) AND the player is the winner, then the caller of this function (the player) is the winner.
	// if the controller is NOT the player controller (If the AI is the one calling EndGame()) AND the player is NOT the winner, then the caller of this function (the AI in this case) is the winner
	// However, IsPlayerController() will always be true because we will always be the player controller by default in this game. So bIsWinner will only be trule if the player is the winner, not the AI
	
        bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
        Controller->GameHasEnded(
            Controller->GetPawn(), /*keep the camera focus on the player*/
            bIsWinner /*bool defining if we won or lost*/
            );
    }
}
```

- Create a method to define whether a AI is dead

ShooterAIController.h
```cpp
public
	bool IsDead() const;
```

ShooterAIController.cpp
```cpp
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
```

### 1.2.3: PawnKilled(): WHO GOT KILLED?

- Implement the PawnKilled() function in KillEmAllGameMode to define which pawn was killed (the player or all the enemies) and call EndGame() passing the result of this function as a parameter.

KillEmAllGameMode.h
```cpp
public:
	//This method is going to override the virtual method in SimpleShooterGameModeBase
	virtual void PawnKilled(APawn* PawnKilled) override;
```

KillEmAllGameMode.cpp
```cpp
void AKillEmAllGameMode::PawnKilled(APawn* PawnKilled)
{
    Super::PawnKilled(PawnKilled);

    UE_LOG(LogTemp, Warning, TEXT("The Pawn was Killed"));
    //In Unreal, change the BP_ShooterGameMode name to BP_KillEmAllGameMode > Open > Class Settings > Change Parent Class to KillEmAllGameMode
        //click on Blueprints in the top bar > GameMode > Select GameMode Base Class > select BP_KillEmAllGameMode

    //GAME OVER: Check if the killed pawn is a player controller (if the player has died) - Lose scenario
    APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());

    if (PlayerController != nullptr)
    {
        EndGame(false);
    }

    //WIN GAME: For loop over all shooterAIs in the world and check if any of them are not dead. then game is not over, so return to exit out of this function and stop executing it
    //If we iterate over all AI and they are all dead then end game - we won
    for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
    {
        bool bIsPlayerController = Controller->IsPlayerController();

        if (!Controller->IsDead())
        {
            return; 
        }
    }
    EndGame(true);
}
```

# 2: Effects:

## 2.1: Widgets:

- In Unreal > Add New > User interface > Widget blueprint > "WBP_LoseScreen" / "WBP_WinScreen" / "WBP_HUDScreen"
- inside WBP_LoseScreen / WBP_LoseScreen > add a text component to the screen > customize it
- inside WBP_HUD > add crosshair and a progress bar and customize them > start percentage of progress bar as 1

![image](https://user-images.githubusercontent.com/12215115/172826273-76efb0aa-52d8-4cf5-8e1a-b10634d81dc9.png)
![image](https://user-images.githubusercontent.com/12215115/172828233-c2afe164-600c-4843-a26f-1bf6af47cf4a.png)
![image](https://user-images.githubusercontent.com/12215115/172828294-22bf847e-c82c-4df7-ab71-d5fe4be2d1ce.png)

- Define which widget class will be spawned
ShooterPlayerController.h
```cpp
public: 
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	//Create a variable of class type of the widget class we are trying to spawn
		//Restrict the type of class to only be User Widget with TSubclassOf<>
	TSubclassOf<class UUserWidget> LoseScreenClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> WinScreenClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDClass; 
	
	UPROPERTY(EditAnywhere)
	UUserWidget* HUD;
```

- Spawn the widgets in the world
ShooterPlayerController.cpp
```cpp
void AShooterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    HUD = CreateWidget(this, HUDClass);
    if (HUD != nullptr)
    {
        HUD->AddToViewport();
    }
    
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health/MaxHealth;
}

void AShooterPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
    Super::GameHasEnded(EndGameFocus, bIsWinner);

    HUD->RemoveFromViewport();

    if (bIsWinner)
    {
        //print win widget
        UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);

        if (WinScreen != nullptr)
        {
            WinScreen->AddToViewport();
        }
        //In Unreal > BP_ShooterPlayerController > WinScreenClass > select our WBP_WinScreen
    }
    else
    {
        //print lose widget

        //Spawn Widget
        UUserWidget* LoseScreen = CreateWidget(
            this, /*Owning object*/ 
            LoseScreenClass /*Widget class*/
            );

        if (LoseScreen != nullptr)
        {
            //Print our widget to the screen
            LoseScreen->AddToViewport();
        }
        //In Unreal > BP_ShooterPlayerController > LoseScreenClass > select our WBP_LoseScreen
    }
}
```
- in WBP_HUD > select the progress bar > details > progress > percent > bind > create biding > rename funciton to "Get Health"
	- righ click > get Owning player pawn > cast to shooter character > Get health percent > return value
- Open SimpleShooter.Build.cs > inside PublicDependencyModuleNames > add "UMG"
- In Unreal > ShooterPlayerController > Details > ShooterPlayerController > Lose Screen Class > select WBP_LoseScreen

## 2.2: Particles

- Spawn emitters on gun shooting and bullet impact

In Gun.h, create emitter components 
```cpp
private:
	UPROPERTY(EditAnywhere)
	UParticleSystem *MuzzleFlash;

	UPROPERTY(EditAnywhere)
	UParticleSystem *ImpactEffect;
```

In Gun.cpp
```cpp
void AGun::PullTrigger()
{
	//Spawn an emitter at the riffle's muzzle on pull trigger
	UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlash, //Particle system variable
		Mesh, //Scene component to which to attach the emitter
		TEXT("MuzzleFlashSocket") //name of the component socket to which to attach
	); 
	//Then in BP_Riffle, select the emitter blueprint for this emitter
	
	if (bSuccess)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		//Then include the impact effect in BP_Rifle
	}
}
```

## 2.3: Sounds

in Gun.h, include sound components 
```cpp
private: 
	UPROPERTY(EditAnywhere)
	USoundBase* MuzzleSound; 

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound; 
```

Gun.cpp
```cpp
void AGun::PullTrigger()
{
	//Spawn sound on pull trigger
	//use SpawnSoundAttach to attach sound to the barrel of our gun and make it come always from that point
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));
	//in BP_Rfile > Details > Gun > MuzzleSound > include our sound effect
	
	if (bSuccess)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
		//in BP_Rfile > Details > Gun > ImpactSound > include our sound effect
	}
}

```

### 2.3.1: Sound Cues: randomize sounds

- In Unreal > add new > Sounds > Sound Cues > A_RifleShot_Cue
- in unreal > content folder > select all sound wave files to be included > A_RifleShot_Cue > right click > select Random: multiple WAVs > link to Modulator > link to Output
- details > random > select randomize without replacement
- details > modulation > select min and max pitch

![image](https://user-images.githubusercontent.com/12215115/173055910-fb7dc378-e397-4a5f-9146-8aa7c09bde08.png)

### 2.3.2: Sound Spacialization: make sounds louder or quieter depending on our distance to them

- in Unreal _ A_RifleShot_Cue > details > attenuation settings > sound attenuation > "ATT_Default" 
- open "ATT_Default" > details > 

### 2.3.3: Music

- In Unreal > Modes > Ambient Sound > drag it into the world > Details > Sound > select from the dropdown : shooter game music

### 2.3.4: Player Speech lines

- Make player say a line on begin play
- BP_PlayerShooterCharacter > Event Graph > Begin Play / Get Actor location > Play Sound at location / select sound asset : "intro"





