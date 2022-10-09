#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"

#include "WidgetListViewTextEntry.generated.h"

UCLASS(BlueprintType)
class MYFIRSTGAMEINUE5_API UWidgetListViewTextEntryData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FDateTime DateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FString String;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetListViewTextEntry* EntryWidget;
};

UCLASS()
class MYFIRSTGAMEINUE5_API UWidgetListViewTextEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* txt_Text = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* txt_DateTime = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* img_Selected = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* img_Hovered = nullptr;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
