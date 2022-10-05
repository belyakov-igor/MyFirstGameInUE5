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

	UFUNCTION(BlueprintCallable, Category = "UI")
	void OpenUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void CloseUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowNextWidget(FName WidgetKey);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPreviousWidget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TArray<TSubclassOf<class UMenuWidget>> WidgetClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	FName StartupWidgetKey = NAME_None;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TArray<class UMenuWidget*> Widgets;
	TArray<class UMenuWidget*> WidgetStack;

	void ShowWidget(TArray<class UMenuWidget*>::SizeType Index);
	void HideWidget();
	TArray<class UMenuWidget*>::SizeType FindWidget(FName Key);
};
