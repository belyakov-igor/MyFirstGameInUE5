#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MenuLogic.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API AMenuLogic : public AActor
{
	GENERATED_BODY()
	
public:	
	AMenuLogic();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void Init(TMap<FName, TSubclassOf<class UMenuWidget>> WidgetClasses, FName StartupWidgetName);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowNextWidget(FName WidgetName);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPreviousWidget();

private:
	FName StartupWidgetName;
	using WidgetArray = TArray<class UMenuWidget*>;

	WidgetArray Widgets;
	WidgetArray WidgetStack;

	void ShowWidget(WidgetArray::SizeType Index);
	void HideWidget();
};
