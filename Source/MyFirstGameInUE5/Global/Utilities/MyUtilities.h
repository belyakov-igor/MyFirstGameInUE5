#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MyUtilities.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FInt32Int32MulticastSignature, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FFloatMulticastSignature, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FTextMulticastSignature, const FText&);
DECLARE_MULTICAST_DELEGATE(FSignalMulticastSignature);

DECLARE_DELEGATE(FSignalSignature);
DECLARE_DELEGATE_OneParam(FInt32Signature, int32);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSignalMulticastDynamicSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIndexedSignalMulticastDynamicSignature, int32, Index);

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

	static bool SpawnAndAddWeaponToCharacter(APawn* Character, TSubclassOf<class ABaseWeapon> WeaponClass, int32 AmmoAmount);
};

UENUM(BlueprintType)
enum ECharacterAnimationSet
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

UCLASS()
class UIndexedSignalObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signal")
	int32 Index;

	UPROPERTY(BlueprintAssignable, Category = "Signal")
	FIndexedSignalMulticastDynamicSignature IndexedSignal;

	UFUNCTION(BlueprintCallable, Category = "Signal")
	void BroadcastIndexedSignal()
	{
		IndexedSignal.Broadcast(Index);
	}
};

USTRUCT(BlueprintType)
struct FDateTimeAndString
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DateTimeAndString")
	FDateTime DateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DateTimeAndString")
	FString String;
};

inline FName HealthComponentName = "HealthComponent";
inline FName StaminaComponentName = "StaminaComponent";
inline FName MeleeWeaponManagerComponentName = "MeleeWeaponManagerComponent";
inline FName RangedWeaponManagerComponentName = "RangedWeaponManagerComponent";
