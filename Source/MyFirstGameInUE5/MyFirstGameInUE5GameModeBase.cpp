#include "MyFirstGameInUE5GameModeBase.h"
#include "Characters/PlayerCharacterBase.h"
#include "Characters/PlayerPlayerControllerBase.h"

AMyFirstGameInUE5GameModeBase::AMyFirstGameInUE5GameModeBase()
{
	DefaultPawnClass = APlayerCharacterBase::StaticClass();
	PlayerControllerClass = APlayerPlayerControllerBase::StaticClass();
}