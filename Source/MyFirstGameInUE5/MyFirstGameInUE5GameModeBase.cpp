#include "MyFirstGameInUE5GameModeBase.h"
#include "Characters/PlayerCharacter.h"
#include "Controllers/MyPlayerState.h"

AMyFirstGameInUE5GameModeBase::AMyFirstGameInUE5GameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
}