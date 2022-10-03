#include "MyGameModeBase.h"
#include "Characters/PlayerCharacter.h"
#include "Controllers/MyPlayerState.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
}
