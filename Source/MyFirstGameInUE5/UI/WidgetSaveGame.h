#pragma once

#include "UI/MenuWidget.h"
#include "Global/Utilities/MyUtilities.h"

#include "CoreMinimal.h"

#include "WidgetSaveGame.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetSaveGame : public UMenuWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_Back = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_Save = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_NewSave = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_Delete = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSaveGameList* wsglist_GameSaves = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UEditableText* editTxt_EditableText = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_EditableTextSave = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetButtonWithText* btn_EditableTextCancel = nullptr;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void ShowEditableText();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void HideEditableText();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SaveGameToSlot(FString SlotName);

	virtual void Refresh_Implementation() override;

private:
	UFUNCTION()
	void Back();

	UFUNCTION()
	void SaveGameToSlotPickedInTheListView();

	UFUNCTION()
	void DeleteSaveForSlotPickedInTheListView();

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void SaveOnEditableTextCommited();

	class UWidgetListViewTextEntryData* GetPickedItemData() const;
};
