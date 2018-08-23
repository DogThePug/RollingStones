// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RollingStonesBall.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"

ARollingStonesBall::ARollingStonesBall()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallMesh(TEXT("/Game/Rolling/Meshes/BallMesh.BallMesh"));

	// Create mesh component for the ball
	Ball = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball0"));
	Ball->SetStaticMesh(BallMesh.Object);
	Ball->BodyInstance.SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Ball->SetSimulatePhysics(true);

	Ball->SetAngularDamping(1.f);
	Ball->SetLinearDamping(1.f);
	Ball->BodyInstance.MassScale = 3.5f;
	Ball->BodyInstance.MaxAngularVelocity = 800.0f;
	Ball->SetNotifyRigidBodyCollision(true);
	
	RootComponent = Ball;
	Ball->SetConstraintMode(EDOFMode::XYPlane);

	// Create a camera boom attached to the root (ball)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bAbsoluteRotation = true; // Rotation of the ball should not affect rotation of boom
	SpringArm->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArm->TargetArmLength = 1200.f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 3.f;

	// Create a camera and attach to boom
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	Tags.Add(FName("Player"));
	
}

void ARollingStonesBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if(bMoving)
	Ball->AddForce(FVector(UpMovement, RightMovement, 0));

	TSet<AActor*> OverlappingActors;

	GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors) {
		UE_LOG(LogTemp, Warning, TEXT("Overlapping"));
		if (OverlappingActor->ActorHasTag(FName("AttractorVolume")) && !bMoving) {
			FVector Direction = OverlappingActor->GetActorLocation() - GetActorLocation();
			float Distance = FVector::Dist(OverlappingActor->GetActorLocation(), GetActorLocation());
			Ball->AddForce(Direction*Distance*500);
		}
	}
}

void ARollingStonesBall::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("MoveRight", IE_Released, this, &ARollingStonesBall::MoveRight);
	PlayerInputComponent->BindAction("MoveForward", IE_Released,this, &ARollingStonesBall::MoveForward);
	PlayerInputComponent->BindAction("MoveLeft", IE_Released, this, &ARollingStonesBall::MoveLeft);
	PlayerInputComponent->BindAction("MoveDown", IE_Released, this, &ARollingStonesBall::MoveDown);


}

void ARollingStonesBall::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other->ActorHasTag(FName("StopTile"))) {
		bMoving = false;
		ResetMovement();
	}
}



void ARollingStonesBall::NotifyActorBeginOverlap(AActor * OtherActor)
{

}

void ARollingStonesBall::ResetMovement()
{
	UpMovement = 0;
	RightMovement = 0;
}

void ARollingStonesBall::MoveRight()
{
	if (!bMoving) {
		RightMovement = ForceApply;
		bMoving = true;
	}
}

void ARollingStonesBall::MoveForward()
{
	if (!bMoving) {
		UpMovement = ForceApply;
		bMoving = true;
	}
}

void ARollingStonesBall::MoveDown()
{
	if (!bMoving) {
		UpMovement = -ForceApply;
		bMoving = true;
	}
}

void ARollingStonesBall::MoveLeft()
{
	if (!bMoving) {
		RightMovement = -ForceApply;
		bMoving = true;
	}
}




