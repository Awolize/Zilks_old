// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.


#include "ZilksArenaField.h"
#include "Zilks.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"

#include "ZilksEntityBase.h"
#include "ZilksEntityUnit.h"

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

	Bases = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Bases"));
	Bases->SetupAttachment(SceneComponent);

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
	Bases->ClearInstances();

	// TODO, this may change the list while iterating over it, no good!
	TArray<AActor*> RemoveActors;
	for (AActor* BaseToRemove : this->Children) {
		if (Cast<AZilksEntityBase>(BaseToRemove)) {
			UE_LOG(LogZilks, Log, TEXT("AZilksArenaField::OnConstruction - Destroy Actor"));
			RemoveActors.Add(BaseToRemove);
		}
	}

	for (auto& actor : RemoveActors)
	{
		actor->Destroy();
	}

	// -----  https://answers.unrealengine.com/questions/143888/how-do-i-remove-items-from-a-tarray-while-iteratin.html ---- ^ might help


	UE_LOG(LogZilks, Log, TEXT("AZilksArenaField::OnConstruction TileContent: size set and reset (size=%d) (this=%x)"), TileContent.Num(), this);


	// Implicit floor with integer division, which makes all room sizes end up being odd.
	int32 HalfPathWidth = PathWidth / 2;
	int32 HalfSize = RoomSize / 2;
	int32 GridSize1 = GridSize; // no idea why this is needed but doesnt work if GridSize is used on its own
	TileContent.Init(nullptr, HalfSize * 2 + 1);

	int32 FullSize = HalfSize * 2 + 1;
	int32 FullPathWidth = HalfPathWidth * 2 + 1;
	UE_LOG(LogZilks, Log, TEXT("FullSize (x): %d"), FullSize);
	UE_LOG(LogZilks, Log, TEXT("FullPathWidth (y):  %d"), FullPathWidth);

	FVector FloorTranslation(0.f, 0.f, 0.f);

	// HalfSize = 9 / 2
	//  for (int32 x = -5; x <= 5; x++)
	for (int32 x = -HalfSize; x <= HalfSize; x++)
	{
		FloorTranslation.X = GridSize1 * x;

		for (int32 y = -HalfPathWidth; y <= HalfPathWidth; y++)
		{
			FloorTranslation.Y = GridSize1 * y;

			Floors->AddInstance(FTransform(FloorTranslation));
		}
	}

	for (int32 x = -HalfSize; x <= HalfSize; x++)
	{
		FloorTranslation.X = GridSize1 * x;
		for (int32 y = -HalfPathWidth; y <= HalfPathWidth; y++)
		{
			FloorTranslation.Y = GridSize1 * y;
			if (y == 0 && (x == -HalfSize || x == HalfSize))
			{
				const FRotator Rotation(0.f, 90.f, 0.f);

				
				const FVector Location = GetActorLocation() + FTransform(FloorTranslation).GetLocation();
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				
				uint32 ListPosition = HalfSize + x;
				TileContent[ListPosition] = GetWorld()->SpawnActor(BaseActor, &Location, &Rotation, SpawnParameters);
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