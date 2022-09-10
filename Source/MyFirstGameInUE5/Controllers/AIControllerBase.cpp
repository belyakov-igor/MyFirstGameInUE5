#include "Controllers/AIControllerBase.h"

#include "Controllers/MyPlayerState.h"
#include "Characters/CharacterBase.h"
#include "Global/Utilities/Components/DamageTakerComponent.h"
#include "AI/Components/MyAIPerceptionComponent.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AAIControllerBase::AAIControllerBase()
{
    MyAIPerceptionComponent = CreateDefaultSubobject<UMyAIPerceptionComponent>("MyAIPerceptionComponent");
    SetPerceptionComponent(*MyAIPerceptionComponent);

    bWantsPlayerState = true;
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    auto PState = Cast<AMyPlayerState>(PlayerState);
    if (PState != nullptr)
    {
        PState->SetTeamID(1);
    }

    InitBlackboardAndBehaviorTree();
    SubscribeToDamageTakerMomentumDelegate();
}

void AAIControllerBase::InitBlackboardAndBehaviorTree()
{
    UBlackboardComponent* BlackboardComp = Blackboard;

    UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp);
    check(BlackboardComp != nullptr);

    BlackboardComp->SetValueAsObject(EnemyActorKeyName, nullptr);
    BlackboardComp->SetValueAsBool(MoveTargetIsMandatoryKeyName, true);
    BlackboardComp->SetValueAsVector(LastDamageTakenDirectionKeyName, FVector(1.f, 0.f, 0.f));
    BlackboardComp->SetValueAsBool(DamageIsTakenRecentlyKeyName, false);
    BlackboardComp->SetValueAsBool(FightHasStartedKeyName, false);
    BlackboardComp->SetValueAsBool(GotShotByUnknownEnemyKeyName, false);

    RunBehaviorTree(BehaviorTreeAsset);
}

void AAIControllerBase::SubscribeToDamageTakerMomentumDelegate()
{
    auto DamageTakerComponent = Cast<UDamageTakerComponent>(GetPawn()->FindComponentByClass<UDamageTakerComponent>());
    check(DamageTakerComponent != nullptr);
    DamageTakerComponent->MomentumTaken.AddDynamic(this, &AAIControllerBase::OnMomentumTaken);
}

void AAIControllerBase::OnMomentumTaken(FName BoneName, FVector ImpactPoint, FVector Momentum)
{
    UBlackboardComponent* BlackboardComp = Blackboard;
    check(BlackboardComp != nullptr);
    auto MomentumDirection = Momentum.GetSafeNormal();
    bool GotShotByUnknownEnemy = true;
    if (auto EnemyActor = Cast<AActor>(BlackboardComp->GetValueAsObject(EnemyActorKeyName)); EnemyActor != nullptr)
    {
        auto EnemyLocation = EnemyActor->GetActorLocation();
        if (auto CharacterBase = Cast<ACharacterBase>(EnemyActor); CharacterBase != nullptr)
        {
            EnemyLocation = CharacterBase->GetCurrentWeaponMuzzleSocketTransform().GetLocation();
        }
        GotShotByUnknownEnemy =
            FVector::DotProduct((EnemyLocation - ImpactPoint).GetSafeNormal(), -MomentumDirection) < 0.95
        ;
    }
    BlackboardComp->SetValueAsBool(GotShotByUnknownEnemyKeyName, GotShotByUnknownEnemy);

    BlackboardComp->SetValueAsVector(LastDamageTakenDirectionKeyName, -MomentumDirection);
    BlackboardComp->SetValueAsBool(DamageIsTakenRecentlyKeyName, true);
    GetWorldTimerManager().SetTimer(
        MomentumTakenTimerHandle
        ,
            [this]
            {
                UBlackboardComponent* BlackboardComp = Blackboard;
                check(BlackboardComp != nullptr);
                BlackboardComp->SetValueAsBool(DamageIsTakenRecentlyKeyName, false);
                BlackboardComp->SetValueAsBool(GotShotByUnknownEnemyKeyName, false);
            }
        , TimeToRememberTakenDamage
        , /*bInLoop*/ false
    );
}

void AAIControllerBase::SetOrderMoveTagetSequence(TArray<MoveTarget> MoveTargets, int32 FirstNonMandatoryTarget, bool LoopTargetSequence)
{
    check(FirstNonMandatoryTarget <= MoveTargets.Num() && FirstNonMandatoryTarget > 0);
    OrderMoveTagetSequence = std::move(MoveTargets);
    FirstNonMandatoryOrderMoveTarget = FirstNonMandatoryTarget;
    bLoopTargetSequence = LoopTargetSequence;
}
