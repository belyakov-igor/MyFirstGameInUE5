#pragma once

#include "Global/Utilities/MyUtilities.h"
#include "UI/WidgetListViewTextEntry.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "WidgetSaveGameList.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetSaveGameList : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	void Refresh();

	UFUNCTION(BlueprintCallable, Category = "UI")
	bool ListIsEmpty() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	UWidgetListViewTextEntryData* GetSelectedEntryData() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UListView* list_GameSaves = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* txt_NoSavedGames = nullptr;


	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnListViewSelectionChanged();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OnListViewItemHovered(const UObject* Item, bool IsHovered);
};
