#include "MyGameModeBase.h"

#include "Global/MyGameInstance.h"
#include "Characters/PlayerCharacter.h"
#include "Controllers/MyPlayerState.h"

AMyGameModeBase::AMyGameModeBase()
{
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	auto MyGameInstance = UMyGameInstance::GetMyGameInstance(this);
	if (
		FName(   UGameplayStatics::GetCurrentLevelName( MyGameInstance->GetWorld() )   )
		== MyGameInstance->MainMenuLevelName
	)
	{
		MyGameInstance->OnMainMenuLoaded();
	}
	else
	{
		MyGameInstance->OnLevelLoaded();
	}
}
