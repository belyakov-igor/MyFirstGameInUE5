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

USTRUCT(BlueprintType)
struct FWeaponUIData
{
    GENERATED_BODY()

        UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
        class UTexture2D* WeaponIcon = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
        class UTexture2D* CrossHairIcon = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
        bool HasAmmo = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
        int32 ArsenalAmount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
        int32 ClipAmount = 0;
};
