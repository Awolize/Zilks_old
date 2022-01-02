// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.


#include "ZilksArenaField.h"
#include "Zilks.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"

#include "ZilksEntityBase.h"
#include "ZilksEntityUnit.h"
#include "Engine/StaticMeshSocket.h"

// Sets default values
AZilksArenaField::AZilksArenaField()
{
	PrimaryActorTick.bCanEverTick = false;
	RoomSize = 5;
	PathWidth = 3;
	GridSize = 400;
	bRebuild = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	Floors = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floors"));
	Floors->SetupAttachment(SceneComponent);
}

#if WITH_EDITOR
void AZilksArenaField::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->HasMetaData("RebuildArenaField"))
		bRebuild = true;

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AZilksArenaField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Only rebuild if needed.
	if (!bRebuild)
		return;

	bRebuild = false;

	UE_LOG(LogZilks, Log, TEXT("AZilksArenaField::OnConstruction Building Room Size %d (this=%x)"), RoomSize, this);


	Floors->ClearInstances();

	// TODO, this may change the list while iterating over it, no good!
	TArray<AActor*> RemoveActors;
	for (AActor* BaseToRemove : this->Children) {
		if (Cast<AZilksEntityBase>(BaseToRemove)) {
			UE_LOG(LogZilks, Log, TEXT("AZilksArenaField::OnConstruction - Destroy Actor"));
			RemoveActors.Add(BaseToRemove);
		}
	}

	for (const auto& actor : RemoveActors)
	{
		actor->Destroy();
	}

	// -----  https://answers.unrealengine.com/questions/143888/how-do-i-remove-items-from-a-tarray-while-iteratin.html ---- ^ might help


	UE_LOG(LogZilks, Log, TEXT("AZilksArenaField::OnConstruction TileContent: size set and reset (size=%d) (this=%x)"), TileContent.Num(), this);


	// Implicit floor with integer division, which makes all room sizes end up being odd.
	const int32 GridSize1 = GridSize; // no idea why this is needed but doesnt work if GridSize is used on its own
	TileContent.Init(nullptr, RoomSize*PathWidth);

	UE_LOG(LogZilks, Log, TEXT("FullSize (x): %d"), RoomSize);
	UE_LOG(LogZilks, Log, TEXT("FullPathWidth (y):  %d"), PathWidth);

	FVector FloorTranslation(0.f, 0.f, 0.f);

	for (uint32 x = 0; x < RoomSize; x++)
	{
		FloorTranslation.X = GridSize1 * x;

		for (uint32 y = 0; y < PathWidth; y++)
		{
			FloorTranslation.Y = GridSize1 * y;

			Floors->AddInstance(FTransform(FloorTranslation));
		}
	}


	for (uint32 x = 0; x < RoomSize; x++)
	{
		FloorTranslation.X = GridSize1 * x;
		for (uint32 y = 0; y < PathWidth; y++)
		{
			FloorTranslation.Y = GridSize1 * y;
			if (y == 0 && (x ==  0 || x == RoomSize))
			{
				const FRotator Rotation(0.f, 90.f, 0.f);

				
				const FVector Location = GetActorLocation() + FTransform(FloorTranslation).GetLocation();
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				
				uint32 ListPosition = (x * PathWidth) + y;
				TileContent[ListPosition] = GetWorld()->SpawnActor(BaseActor, &Location, &Rotation, SpawnParameters);
				if (!TileContent[ListPosition]) // Could not spawn actor for some reason.
					continue;
				
				TileContent[ListPosition]->SetActorLabel((x < 0) ? "ZilksEntityBase Player 1" : "ZilksEntityBase Player 2");
				TileContent[ListPosition]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
				

				UE_LOG(LogZilks, Log, TEXT("x: %d, y: %d, count: %d"), x, y, ListPosition);
			}
		}
	}

	
	/*
	int count = -1;
	for (int32 x = -HalfSize; x <= HalfSize; x++)
	{
		count++;
		FloorTranslation.X = GridSize1 * x;
		for (int32 y = -HalfPathWidth; y <= HalfPathWidth; y++)
		{
			// Bases
			FRotator Rotation(0.f, 90.f, 0.f);
			FVector Translation = FloorTranslation;

			if (y == 0 && x == HalfSize) // if in the middle of the grid (on the y axis)
			{
				Bases->AddInstance(FTransform(Rotation, Translation));
				// Create Base Actor

				const FVector Location = FTransform(FloorTranslation).GetLocation();
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;

				TileContent[count] = GetWorld()->SpawnActor(BaseActor, &Location, &Rotation, SpawnParameters);
				TileContent[count]->SetActorLabel("ZilksEntityBase Player 1");
				//AActor* BasePawn = SpawnActor<AActor>(BaseActor, FTransform(FloorTranslation).GetLocation(), , NULL, NULL);
				// TileContent[x * y] = ABase();
			}

			else if (y == 0 && x == -HalfSize) // if in the middle of the grid (on the y axis)
			{
				Bases->AddInstance(FTransform(Rotation, Translation));
				// Create Base Actor

				const FVector Location = FTransform(FloorTranslation).GetLocation();
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;

				TileContent[count] = GetWorld()->SpawnActor(BaseActor, &Location, &Rotation, SpawnParameters);
				TileContent[count]->SetActorLabel("ZilksEntityBase Player 2");
				//AActor* BasePawn = SpawnActor<AActor>(BaseActor, FTransform(FloorTranslation).GetLocation(), , NULL, NULL);
				// TileContent[x * y] = ABase();
			}
		}
	}*/

}

void AZilksArenaField::Destroyed()
{
	Super::Destroyed();

	// TODO, this may change the list while iterating over it, no good!
	TArray<AActor*> RemoveActors;
	for (AActor* BaseToRemove : this->Children) {
		if (Cast<AZilksEntityBase>(BaseToRemove)) {
			UE_LOG(LogZilks, Log, TEXT("AZilksArenaField::OnConstruction - Destroy Actor"));
			RemoveActors.Add(BaseToRemove);
		}
	}
	for (AActor* const& actor : RemoveActors)
	{
		actor->Destroy();
	}
	// -----  https://answers.unrealengine.com/questions/143888/how-do-i-remove-items-from-a-tarray-while-iteratin.html ---- ^ might help
}
