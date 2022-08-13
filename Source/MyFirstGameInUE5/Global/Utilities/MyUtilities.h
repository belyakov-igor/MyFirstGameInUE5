#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MyUtilities.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FInt32Int32MulticastSignature, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FFloatSignature, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FTextSignature, const FText&);
DECLARE_MULTICAST_DELEGATE(FSignalMulticastSignature);

DECLARE_DELEGATE(FSignalSignature);

UCLASS()
class MYFIRSTGAMEINUE5_API UMyUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "General")
	static void FindByValue(const TMap<FString, FString>& map, const FString& value, FString& ret, bool& succeeded);

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

UENUM(BlueprintType)
enum EPlayerCharacterBaseAnimationSet
{
	Unarmed     UMETA(DisplayName = "Unarmed"),
	Pistol      UMETA(DisplayName = "Pistol"),
	Rifle       UMETA(DisplayName = "Rifle"),
};

USTRUCT(BlueprintType)
struct MYFIRSTGAMEINUE5_API FWeaponUIData
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

inline FName HealthComponentName = "HealthComponent";
inline FName StaminaComponentName = "StaminaComponent";
inline FName MeleeWeaponManagerComponentName = "MeleeWeaponManagerComponent";
inline FName RangedWeaponManagerComponentName = "RangedWeaponManagerComponent";