#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyUtilities.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FInt32Int32MulticastSignature, int32, int32);
DECLARE_MULTICAST_DELEGATE(FSignalMulticastSignature);
DECLARE_DELEGATE(FSignalSignature);

UCLASS()
class MYFIRSTGAMEINUE5_API UMyUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "General")
	static void FindByValue(const TMap<FString, FString>& map, const FString& value, FString& ret, bool& succeeded);
};
