# 3rdPersonV2

# Project Steps:
1. Create Components: Parent Classes
2. Create Sub-Components: Child Classes
3. Player Input: Moving the Actor, Firing
4. Player Animation
5. Actions and Events: Hit Events, Health Component, Apply Damage
6. Game Rules, Game Mode and Game Controller (Game Cycle: Start > Death > Winning, Loosing > End)
7. Special Effects (Sounds, Particles)


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


# 2: Create Sub-Components: Gun Component

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
```cpp
private: 
	UPROPERTY(EditDefaultsOnly) /*so that no one can edit it at run time*/
	//To Spawn an object in the world from C++ we need to connect the C++ code to the Unreal Blueprint that contains the object's mesh. To do this we use TSubclassOf<>.
		//TSubclassOf<> makes the connection between the C++ code and the blueprint. (Reflection)
	//Use TSubclassOf<> to declare a variable that represents the type of the class of the object we want to spawn in the world
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	//Declare an instance of the class (pointer) Gun for us to store our Gun variable
	AGun* Gun;
```

cpp
```cpp
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Spawn the rifle component from our actor component at run time
	Gun = GetWorld()->SpawnActor<AGun>(GunClass); //Then select the BP_Rifle as the GunClass Mesh in our BP_ShotterCharacter
	
	//Hide the gun that is currently there
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None); //Then, in the Skeleton asset, add a new socket
	
	//Attach the new gun to the actor's hand
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	
	//Set up the gun to have this class as its owner. allows the gun actor to know that the ShooterCharacter is its owner. Useful to assign who is generating damage by shooting which gun. 
	Gun->SetOwner(this);
	
	//Then go to Unreal, set the rifle in the right position in the world copy the location coordinates, then open BP_Rifle and paste the coordinates in the mesh component
}
```


# 3: Player Input: 

## 3.1: Settup an axis mapping for movement and an action mapping for firing  

- Unreal > Edit > Project Settings > Input > Bindings > Axis Mapping / Action Mapping
- Click add and create one axis mapping function Move to move forwards and backwards: assign one key for forwards with value +1 and one key for backwards with value -1.
- Click add and create one axis mapping function LookUp: assign one key to look up (mouse Y) with value -1.
- Also create one function Turn for turning left and right and look left and right and assign its respective keys with respective values
- Click add and create one action mapping function for fire and for jump and assign a key to it.

## 3.2: Bind the axis / action mapping to our action callback functions

### 3.2.1: Movement actions

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
- Define the action callback functions to Move forward, move right

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
```

- In unreal > select BP_PawnPlayer > physics > set simulate physics off for both the CapsuleComponent and the BaseMesh
- Make sure you move the capsule and the base mesh a little above the ground so that they don't get stuck in the terrain

### 3.2.2: Fire / Shoot actions

- Create the pull trigger function inside Gun c++ 

Gun.h
```cpp
public:	
	// Sets default values for this actor's properties
	AGun();

	void PullTrigger();
```

Gun.cpp
```cpp
void AGun::PullTrigger()
{
	UE_Log(LogTemp, Warning, TEXT("You have pulled the trigger"));
}
```

- Call the pull trigger function from inside the ShooterCharacter c++

ShooterCharacter.h
```cpp
public:
	void Shoot();
```

ShooterCharacter.cpp
```cpp
void AShooterCharacter::Shoot()
{
	Gun->PullTrigger();
}
```

# 4: Player Animation 

- Create an Animation Blueprint: In Unreal, Add new > Animation > Animation Blueprint > select target skeleton > select that of our character (wraith) > call it ABP_ShooterCharacter
- in BP_ShooterCharacter > Details > Animation > Anim Class > select our own custom animation blueprint: ABP_ShooterCharacter

## 4.1: Set the blueprint logic for character movement

in ABP_ShooterCharacter > Event Graph:

Use Try Get Pawn Owner to get the pawn object (BP_ShooterCharacter) and get info from the pawn about its movement.

- Set Speed: TryGetPawnOwner >  GetVelocity > VectorLength > SetSpeed
- Set Angle: TryGetPawnOwner > GetActorTransform / GetVelocity > InverseTransformDirection > RotationFromXVector > SetAngle
	- GetActorTransform: transform to rotate the pawn by indicating where the pawn is in the world
	- InverseTransformDirection: velocity is in global space and we should convert it to local space in order to get the angle (direction) to which the player should be going
	- RotationFromXVector: set the yawn to know how far it is turning to the right or left. (Right click on Return Value, select "split struct pin" to get only the return for the yaw.

Use Event Blueprint Update Animation to get update information on the pawn movements

- Set the character's aim: 
TryGetPawnOwner > GetControlRotation / GetActorRotation > Delta (Rotator): return valueY > SetAimPitch
EventBlueprintUpdateAnimation > ?IsValid > Sequence > Execution > SetAimPitch
	-  Delta (Rotator): control rotation is global. we need to find the delta distance btw the actor's rotation and the global control rotation

- Set Jump movement, check whether the character is jumping or falling
TryGetPawnOwner > CastToCharacter > IsFalling > (create an IsAirBorne bool) > SetIsAirBorne
EventBlueprintUpdateAnimation > ?IsValid > Sequence > Execution > SetIsAirBorne

![image](https://user-images.githubusercontent.com/12215115/170245151-18316618-b9a5-4017-9e07-7052556f03cb.png)
 
(FALTOU CONNECTAR NA BLUEPRINT NODE DO ISDEAD PQ AINDA TEM QUE CRIAR FUNÇAO ISDEAD NO CPP - SEÇÃO HEALTH COMPONENTS)

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
bool AGun::GunTrace(
	FHitResult& Hit, /*a hit result out param (will be modified by this function), with an & because it is a reference to this variable's address*/
	FVector& ShotDirection /*a shot direction out param (will be modified by this function)*/
	)
{
	AController* OwnerController = GetOwnerController();

	if (OwnerController == nullptr) return false;

	FVector Location; //Line trace start point
	FRotator Rotation;

	OwnerController->GetPlayerViewPoint(Location, Rotation); //Location and Rotation are out parameters because are being modified inside this function

	//Get where the shot was coming from, which is the oposite of the shot direction coming our of the gun
	ShotDirection = - Rotation.Vector();

	//Create a line trace for the bullet that tells us which object it has hit

	//Create an endpoint for our line trace
		//End point of a vector = Start point + range of the vector * direction of the vector
	FVector End = Location + Rotation.Vector() * MaxRange;

	//Define actors that should be ignore by the gun line tracing so that we don't shoot ourselves and pass these as params in the line trace method below
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	//Line trace by channel makes the system search through this line and define which types of object can block our bullet and which can't
		//Set our custom trace channel in Project Settings > Collision > Trace Channels > define presets to define interaction with each type of objects
		//Fetch the enum of the collision channel for our bullet in Project Folder > Config > Default Engine > search for the name of the custom collision channel we crated, "Bullet" and see which channel was asigned to it
	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
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
	- Call TakeDamage function

Gun.cpp
```cpp
void AGun::PullTrigger()
{
	if (bSuccess)
	{
		//Then include the impact effect in BP_Rifle

		AActor* HitActor = Hit.GetActor();

		//Get the actor to be damaged
		if (Hit.GetActor() != nullptr)
		{
			AController* Instigator = Cast<AController>(HitActor->GetInstigator()); 

			//Apply Damange
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr); //this last param allows to define a specifit TSubclassOf UDamageType, but since we are not using it we will just pass a nullptr
			
			AController* OwnerController = GetOwnerController();

			HitActor->TakeDamage(
				Damage, 
				DamageEvent, 
				OwnerController /*the controller of this pawn is also the instigator of the applyed damaged*/,
			 	this); 
		}
	}
}
```

 *** PAREI EM VIRTUAL METHODS IN C++ ****
























































