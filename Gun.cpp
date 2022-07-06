// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

//Types of pointer operators:
	//&: Address Operator: returns the memory address of the variable
	//*: Value Operator: returns the value of object pointed by a pointer variable


// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Construct our components 
		//set the root component
		//set the mesh component and attach it to the root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AGun::PullTrigger()
{

	//Spawn an emitter at the riffle's muzzle on pull trigger
	UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlash, //Particle system variable
		Mesh, //Scene component to which to attach the emitter
		TEXT("MuzzleFlashSocket") //name of the component socket to which to attach
	); 
	//Then in BP_Riffle, select the emitter blueprint for this emitter

	//Spawn sound on pull trigger
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));

	FHitResult Hit;
	FVector ShotDirection;
	bool bSuccess = GunTrace(Hit, ShotDirection);

	if (bSuccess)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);

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

