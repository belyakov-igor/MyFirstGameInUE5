#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"

#include "EnvQueryTest_TraceToEnemyActor.generated.h"

UCLASS()
class MYFIRSTGAMEINUE5_API UEnvQueryTest_TraceToEnemyActor : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	explicit UEnvQueryTest_TraceToEnemyActor(const FObjectInitializer& ObjectInitializer);

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	/** context: other end of trace test */
	UPROPERTY(EditDefaultsOnly, Category=Trace)
	TSubclassOf<UEnvQueryContext> Context;
};
