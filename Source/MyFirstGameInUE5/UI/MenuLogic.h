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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FName StartupWidgetName;

	UPROPERTY()
	TArray<class UMenuWidget*> Widgets;
	TArray<class UMenuWidget*> WidgetStack;

	void ShowWidget(TArray<class UMenuWidget*>::SizeType Index);
	void HideWidget();
};
