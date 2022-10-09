#pragma once

#include "UI/MenuWidget.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UI")
	bool IsUIOpen() const;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowNextWidget(TEnumAsByte<EMenuWidget> WidgetKey);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowPreviousWidget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TArray<TSubclassOf<class UMenuWidget>> WidgetClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TEnumAsByte<EMenuWidget> StartupWidgetKey = EMenuWidget_Invalid;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TArray<UMenuWidget*> Widgets;
	TArray<UMenuWidget*> WidgetStack;

	void ShowWidget(TArray<class UMenuWidget*>::SizeType Index);
	void HideWidget();
	TArray<UMenuWidget*>::SizeType FindWidget(EMenuWidget Key);
};
