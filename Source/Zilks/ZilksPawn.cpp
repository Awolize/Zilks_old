// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

#include "ZilksPawn.h"
#include "Zilks.h"
#include "ZilksPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"

AZilksPawn::AZilksPawn()
{
	Collision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetVisibleFlag(false);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Collision);

	Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(Body);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Collision);
	SpringArm->SetRelativeLocation(FVector(120.f, 0.f, 50.f));
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 600.f;
	SpringArmLengthScale = 2000.f;
	SpringArmLengthMin = 0.f;
	SpringArmLengthMax = 1000.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(15.f, 0.f, 0.f));
	Camera->PostProcessSettings.bOverride_MotionBlurAmount = true;
	Camera->PostProcessSettings.MotionBlurAmount = 0.1f;

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	Movement->MaxSpeed = 5000.f;
	Movement->Acceleration = 5000.f;
	Movement->Deceleration = 10000.f;

	MoveScale = 1.f;
	RotateScale = 50.f;
	bFreeFly = false;
	PrimaryActorTick.bCanEverTick = true;
	ZMovementFrequency = 2.f;
	ZMovementAmplitude = 5.f;
	TiltMax = 15.f;
	TiltMoveScale = 0.6f;
	TiltRotateScale = 0.4f;
	TiltResetScale = 0.3f;
}

// Called when the game starts or when spawned
void AZilksPawn::BeginPlay()
{
	Super::BeginPlay();

	bool bSpawnUnit = false;
	if (bSpawnUnit)
	{
		const FVector SpawnOffset(100.f, 0.f, 1500.f);
		const FVector SpawnLocation = GetActorTransform().TransformPosition(SpawnOffset);
		const FTransform SpawnTransform(FQuat::Identity, SpawnLocation);

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = this;

		/*
		AZilksPawn* Anvil = GetWorld()->SpawnActor<AZilksPawn>(AnvilClass, SpawnTransform, SpawnInfo);
		if (Anvil) {
			Anvil->BeingFalling();
		}
		*/
	}

}

void AZilksPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (!PlayerInputComponent)
	{
		UE_LOG(LogZilks, Error, TEXT("AZilksPawn::SetupPlayerInputComponent - Dont forget to set the input settings to EnhancedPlayerInput togehter with the EnhancedPlayerComponent"));
		return;
	}

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	AZilksPlayerController* ZPC = Cast<AZilksPlayerController>(Controller);
	check(EIC && ZPC);
	EIC->BindAction(ZPC->MoveAction, ETriggerEvent::Triggered, this, &AZilksPawn::Move);
	EIC->BindAction(ZPC->RotateAction, ETriggerEvent::Triggered, this, &AZilksPawn::Rotate);
	EIC->BindAction(ZPC->FreeFlyAction, ETriggerEvent::Started, this, &AZilksPawn::ToggleFreeFly);
	EIC->BindAction(ZPC->SpringArmLengthAction, ETriggerEvent::Triggered, this,
		&AZilksPawn::UpdateSpringArmLength);

	ULocalPlayer* LocalPlayer = ZPC->GetLocalPlayer();
	check(LocalPlayer);
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(ZPC->PawnMappingContext, 0);
}

void AZilksPawn::Move(const FInputActionValue& ActionValue)
{
	FVector Input = ActionValue.Get<FInputActionValue::Axis3D>();
	// UFloatingPawnMovement handles scaling this input based on the DeltaTime for this frame.
	AddMovementInput(GetActorRotation().RotateVector(Input), MoveScale);
	TiltInput += Input.Y * TiltMoveScale * MoveScale;
}

void AZilksPawn::Rotate(const FInputActionValue& ActionValue)
{
	FRotator Input(ActionValue[0], ActionValue[1], ActionValue[2]);
	Input *= GetWorld()->GetDeltaSeconds() * RotateScale;
	TiltInput += Input.Yaw * TiltRotateScale;

	if (bFreeFly) {
		AddActorLocalRotation(Input);
	}
	else {
		Input += GetActorRotation();
		Input.Pitch = FMath::ClampAngle(Input.Pitch, -89.9f, 89.9f);
		Input.Roll = 0;
		SetActorRotation(Input);
	}
}

void AZilksPawn::ToggleFreeFly()
{
	bFreeFly = !bFreeFly;
}

void AZilksPawn::UpdateSpringArmLength(const FInputActionValue& ActionValue)
{
	SpringArm->TargetArmLength += ActionValue[0] * GetWorld()->GetDeltaSeconds() * SpringArmLengthScale;
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength,
		SpringArmLengthMin, SpringArmLengthMax);
}

void AZilksPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Add Z Movement.
	if (ZMovementAmplitude)
	{
		ZMovementTime += DeltaSeconds;
		float ZMovement = FMath::Sin(ZMovementTime * ZMovementFrequency) * ZMovementAmplitude;
		Body->SetRelativeLocation(FVector(0.f, 0.f, ZMovement + ZMovementOffset));
	}

	// Add body and head tilting.
	FRotator Rotation = Body->GetRelativeRotation();

	if (TiltInput != 0.f)
	{
		Rotation.Roll = FMath::Clamp(Rotation.Roll + TiltInput, -TiltMax, TiltMax);
		TiltInput = 0.f;
	}

	// Always try to tilt back towards the center.
	if (Rotation.Roll > 0.f)
	{
		Rotation.Roll -= TiltResetScale;
		if (Rotation.Roll < 0.f)
			Rotation.Roll = 0.f;
	}
	else if (Rotation.Roll < 0.f)
	{
		Rotation.Roll += TiltResetScale;
		if (Rotation.Roll > 0.f)
			Rotation.Roll = 0.f;
	}

	Body->SetRelativeRotation(Rotation);
	Head->SetRelativeRotation(FRotator(0.f, Rotation.Roll, 0.f));
}