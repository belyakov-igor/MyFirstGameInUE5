// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyUtilities.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UMyUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "General")
	static void FindByValue(const TMap<FString, FString>& map, const FString& value, FString& ret, bool& succeeded);
};
