// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.


#include "ZilksGameMode.h"
#include "Zilks.h"

AZilksGameMode::AZilksGameMode() {

}

void AZilksGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UE_LOG(LogZilks, Log, TEXT("Game is running: %s %s"), *MapName, *Options);
}
