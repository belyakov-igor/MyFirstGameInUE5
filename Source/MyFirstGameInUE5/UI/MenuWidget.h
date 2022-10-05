#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MenuWidget.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMenuLogic(class AMenuLogic* NewMenuLogic) { MenuLogic = NewMenuLogic; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FName Key = NAME_None;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class AMenuLogic* MenuLogic = nullptr;
};
