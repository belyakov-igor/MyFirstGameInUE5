#include "AICharacter.h"

DEFINE_LOG_CATEGORY_STATIC(AICharacter, All, All);

AAICharacter::AAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

