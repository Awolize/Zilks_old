// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ZilksEntity.generated.h"

UCLASS()
class ZILKS_API AZilksEntity : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AZilksEntity();

	/** Handle input to update location. */
	void Move();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Grid Coord Pos */
	FVector2D GridPos;
};
