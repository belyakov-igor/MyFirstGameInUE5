#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "WeaponUtilities.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UWeaponUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Weapon")
	static void AddWeaponToCharacter(class ABaseWeapon* Weapon, class AActor* Character);

    template <typename T> inline
    static T* FindNotifyByClass(class UAnimSequenceBase* Animation)
    {
        if (Animation == nullptr)
            return nullptr;
        const auto NotifyEvents = Animation->Notifies;
        for (const auto& Event : NotifyEvents)
        {
            auto AnimNotify = Cast<T>(Event.Notify);
            if (AnimNotify != nullptr)
            {
                return AnimNotify;
            }
        }
        return nullptr;
    }
};
