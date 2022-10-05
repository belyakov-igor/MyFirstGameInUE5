#pragma once

#include "UI/MenuWidget.h"

#include "CoreMinimal.h"

#include "WidgetPauseMenu.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetPauseMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_continue = nullptr;
};
