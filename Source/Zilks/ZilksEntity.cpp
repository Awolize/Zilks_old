// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.


#include "ZilksEntity.h"

// Sets default values
AZilksEntity::AZilksEntity()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AZilksEntity::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AZilksEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AZilksEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AZilksEntity::Move()
{
	// Get grid actor to see possible tiles
	// Move as long as possible, towards enemy team
	// call grid_actor to sync movement
}
