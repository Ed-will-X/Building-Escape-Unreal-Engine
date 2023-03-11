// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"	// Must be at the bottom

#define OUT	// Removed upon compilation


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find physics actor component on: %s"), *(GetOwner()->GetName()));
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab button pressed"));

	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	if (HitResult.GetActor())
	{
		if (!PhysicsHandle) { return; }

		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach()	// Line trace end
		);
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{
		// Changes the position of the grabbed component on every frame
		PhysicsHandle->SetTargetLocation(GetPlayersReach());
	}
}


FVector UGrabber::GetPlayersReach() const
{

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);


	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}


FVector UGrabber::GetPlayersWorldPosition() const
{

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	return PlayerViewPointLocation;
}



FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{

	// Getting actors hit with a line trace
	FHitResult Hit;
	FCollisionQueryParams TraceParams(
		FName(TEXT("")),	// Tag for collision, we're not using tags here, we're using the e-collision channel set to physics body
		false,	// Whether or not we're using complex collision
		GetOwner()	// Ignores current player from collision
	);

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPosition(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),	// Collision Channel
		TraceParams
	);

	AActor* ActorHit = Hit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Hit: %s"), *(ActorHit->GetName()));
	}

	return Hit;
}