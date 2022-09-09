#pragma once

#include "Characters/CharacterBase.h"

#include "CoreMinimal.h"

#include "AICharacter.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AAICharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AAICharacter();

	virtual FRotator GetAimRotation() const override { return AimRotation; }
	void SetAimRotation(FRotator Rotator) { AimRotation = Rotator; }

private:
	FRotator AimRotation = FRotator::ZeroRotator;
};
