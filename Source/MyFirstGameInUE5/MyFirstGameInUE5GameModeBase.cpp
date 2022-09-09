#include "MyFirstGameInUE5GameModeBase.h"
#include "Characters/PlayerCharacter.h"

AMyFirstGameInUE5GameModeBase::AMyFirstGameInUE5GameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
}