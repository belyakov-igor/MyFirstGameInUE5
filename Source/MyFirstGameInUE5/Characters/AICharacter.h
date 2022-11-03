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

	void Crouch(bool Crouch);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AICharacter")
	static bool IsThereAnyLivingAICharacter();

protected:
	virtual void Die() override;
};
