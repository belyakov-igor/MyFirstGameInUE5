#pragma once

#include "UI/MenuWidget.h"
#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"

#include "WidgetLoadGame.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetLoadGame : public UMenuWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_Back = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_Load = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_Delete = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSaveGameList* wsglist_GameSaves = nullptr;

	virtual void Refresh_Implementation() override;

private:
	UFUNCTION()
	void Back();

	UFUNCTION()
	void LoadGameFromSlotPickedInTheListView();

	UFUNCTION()
	void DeleteSaveForSlotPickedInTheListView();

	class UWidgetListViewTextEntryData* GetPickedItemData() const;
};
