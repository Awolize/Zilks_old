// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.


#include "ZilksArenaField.h"
#include "Zilks.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"

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


	// Implicit floor with integer division, which makes all room sizes end up being odd.
	int32 HalfPathWidth = PathWidth / 2;
	int32 HalfSize = RoomSize / 2;
	int32 GridSize1 = GridSize; // no idea why this is needed but doesnt work if GridSize is used on its own

	FVector FloorTranslation(0.f, 0.f, 0.f);

	for (int32 x = -HalfSize; x <= HalfSize; x++)
	{
		FloorTranslation.X = GridSize1 * x;

		for (int32 y = -HalfPathWidth; y <= HalfPathWidth; y++)
		{
			// Floors
			FloorTranslation.Y = GridSize1 * y;
			Floors->AddInstance(FTransform(FloorTranslation));


			// Bases
			FRotator Rotation(0.f, 90.f, 0.f);
			FVector Translation = FloorTranslation;

			if (y == 0 && abs(x) == HalfSize) // if in the middle of the grid (on the y axis)
			{
				if (x == HalfSize) {
					float FloorInstanceSize = Floors->GetStaticMesh()->GetBounds().BoxExtent.X * 2; // get Floor mesh size to offset the placement with it. * 2 to get the entire size and not just half
					Translation.X += FloorInstanceSize;
				}

				Bases->AddInstance(FTransform(Rotation, Translation));
			}
		}
	}





}