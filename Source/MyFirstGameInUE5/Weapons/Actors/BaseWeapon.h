#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BaseWeapon.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	virtual void BeginAttack() {};
	virtual void EndAttack() {};

	bool AttackIsBeingPerformed() const { return bAttackIsBeingPerformed; }

protected:
	bool bAttackIsBeingPerformed = false;

	APlayerController* GetPlayerController() const;
};
