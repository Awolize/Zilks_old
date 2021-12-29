// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZilksArenaField.generated.h"

UCLASS()
class ZILKS_API AZilksArenaField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZilksArenaField();

#if WITH_EDITOR
	/** Check properties that change to see if we need to rebuild. */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void OnConstruction(const FTransform& Transform) override;

	/** Static mesh to use for the 2 player bases. */
	UPROPERTY(EditAnywhere)
	class UInstancedStaticMeshComponent* Bases;

	/** Static mesh to use for walls. */
	UPROPERTY(EditAnywhere)
	class UInstancedStaticMeshComponent* Floors;

	/** Size of grid to use when placing meshes. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 3, ClampMax = 25, RebuildArenaField))
	uint32 RoomSize;

	/** Size of grid to use when placing meshes. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 1, ClampMax = 5, RebuildArenaField))
	uint32 PathWidth;

	/** Size between meshes. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 400, ClampMax = 1000, RebuildArenaField))
	uint32 GridSize;


private:
	/** Whether we need to rebuild or not. */
	int32 bRebuild : 1;
};
