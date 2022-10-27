#include "Controllers/AIControllerBase.h"

#include "Controllers/MyPlayerState.h"
#include "Characters/CharacterBase.h"
#include "Characters/AICharacter.h"
#include "Global/Utilities/Components/DamageTakerComponent.h"
#include "AI/Components/MyAIPerceptionComponent.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"

AAIControllerBase::AAIControllerBase()
{
    MyAIPerceptionComponent = CreateDefaultSubobject<UMyAIPerceptionComponent>("MyAIPerceptionComponent");
    SetPerceptionComponent(*MyAIPerceptionComponent);

    bWantsPlayerState = true;
}

TArray<uint8> AAIControllerBase::GetActorSaveData_Implementation()
{
    SavedPossessedCharacterName = GetPawn()->GetFName();

    if (Blackboard != nullptr)
    {
        auto EnemyActor = Blackboard->GetValueAsObject(EnemyActorKeyName);
        SavedBlackboardKeys.EnemyActorName = EnemyActor != nullptr ? EnemyActor->GetFName() : NAME_None;
        SavedBlackboardKeys.bMoveTargetIsMandatory = Blackboard->GetValueAsBool(MoveTargetIsMandatoryKeyName);
        SavedBlackboardKeys.LastDamageTakenDirection = Blackboard->GetValueAsVector(LastDamageTakenDirectionKeyName);
        SavedBlackboardKeys.bDamageIsTakenRecently = Blackboard->GetValueAsBool(DamageIsTakenRecentlyKeyName);
        SavedBlackboardKeys.DamageIsTakenRecentlyTime =
            GetWorldTimerManager().IsTimerActive(MomentumTakenTimerHandle)
            ? GetWorldTimerManager().GetTimerRemaining(MomentumTakenTimerHandle)
            : -1.f
        ;
        SavedBlackboardKeys.bFightHasStarted = Blackboard->GetValueAsBool(FightHasStartedKeyName);
        SavedBlackboardKeys.bGotShotByUnknownEnemy = Blackboard->GetValueAsBool(GotShotByUnknownEnemyKeyName);
    }

    return ISavable::GetActorSaveData_Implementation();
}

void AAIControllerBase::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> Actors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAICharacter::StaticClass(), Actors);

    for (auto Actor : Actors)
    {
        auto AICharacter = Cast<AAICharacter>(Actor);
        if (AICharacter == nullptr)
        {
            check(false);
            continue;
        }
        if (Actor->GetFName() == SavedPossessedCharacterName)
        {
            Possess(AICharacter);
            break;
        }
    }
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

    AActor* EnemyActor = nullptr;
    if (SavedBlackboardKeys.EnemyActorName != NAME_None)
    {
        TArray<AActor*> Actors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Actors);
        for (auto Actor : Actors)
        {
            if (Actor->GetFName() == SavedBlackboardKeys.EnemyActorName)
            {
                EnemyActor = Actor;
                break;
            }
        }
    }

    BlackboardComp->SetValueAsObject(EnemyActorKeyName, EnemyActor);
    BlackboardComp->SetValueAsBool(MoveTargetIsMandatoryKeyName, SavedBlackboardKeys.bMoveTargetIsMandatory);
    BlackboardComp->SetValueAsVector(LastDamageTakenDirectionKeyName, SavedBlackboardKeys.LastDamageTakenDirection);
    BlackboardComp->SetValueAsBool(DamageIsTakenRecentlyKeyName, SavedBlackboardKeys.bDamageIsTakenRecently);
    if (SavedBlackboardKeys.DamageIsTakenRecentlyTime >= 0.f)
    {
        GetWorldTimerManager().SetTimer(
            MomentumTakenTimerHandle
            , this
            , &AAIControllerBase::MomentumTakenIsNotRecentAnymore
            , SavedBlackboardKeys.DamageIsTakenRecentlyTime
            , /*bInLoop*/ false
        );
    }
    BlackboardComp->SetValueAsBool(FightHasStartedKeyName, SavedBlackboardKeys.bFightHasStarted);
    BlackboardComp->SetValueAsBool(GotShotByUnknownEnemyKeyName, SavedBlackboardKeys.bGotShotByUnknownEnemy);

    RunBehaviorTree(BehaviorTreeAsset);
}

void AAIControllerBase::SubscribeToDamageTakerMomentumDelegate()
{
    auto DamageTakerComponent = GetPawn()->FindComponentByClass<UDamageTakerComponent>();
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
        , this
        , &AAIControllerBase::MomentumTakenIsNotRecentAnymore
        , TimeToRememberTakenDamage
        , /*bInLoop*/ false
    );
}

void AAIControllerBase::MomentumTakenIsNotRecentAnymore()
{
    UBlackboardComponent* BlackboardComp = Blackboard;
    check(BlackboardComp != nullptr);
    BlackboardComp->SetValueAsBool(DamageIsTakenRecentlyKeyName, false);
    BlackboardComp->SetValueAsBool(GotShotByUnknownEnemyKeyName, false);
}

void AAIControllerBase::SetOrderMoveTagetSequence(FMoveTargetSequenceTaskData NewMoveTargetSequenceTaskData)
{
    check(
        NewMoveTargetSequenceTaskData.FirstNonMandatoryOrderMoveTarget
        <= NewMoveTargetSequenceTaskData.OrderMoveTagetSequence.Num()
    );
    check(NewMoveTargetSequenceTaskData.FirstNonMandatoryOrderMoveTarget >= 0);
    MoveTargetSequenceTaskData = std::move(NewMoveTargetSequenceTaskData);
}
