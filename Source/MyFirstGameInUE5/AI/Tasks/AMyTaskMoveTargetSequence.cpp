#include "AI/Tasks/AMyTaskMoveTargetSequence.h"

#include "Controllers/AIControllerBase.h"

#include "BehaviorTree/BlackboardComponent.h"

UAMyTaskMoveTargetSequence::UAMyTaskMoveTargetSequence()
{
	bCreateNodeInstance = true;
	bNotifyTick = false;
	NodeName = "Follow move target sequence";
}

EBTNodeResult::Type UAMyTaskMoveTargetSequence::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
{
	Controller = Cast<AAIControllerBase>(OwnerComp.GetAIOwner());
	if (Controller == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	UpdateBlackBoardKeyTargetIsMandatory();
	return Move(OwnerComp);
}

EBTNodeResult::Type UAMyTaskMoveTargetSequence::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller->ReceiveMoveCompleted.RemoveDynamic(this, &UAMyTaskMoveTargetSequence::OnMoveCompleted);
	Controller->StopMovement();
	Controller->ClearFocus(EAIFocusPriority::Gameplay);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	return EBTNodeResult::Aborted;
}

EBTNodeResult::Type UAMyTaskMoveTargetSequence::Move(UBehaviorTreeComponent& OwnerComp)
{
	check(Controller != nullptr);

	const auto& Sequence = Controller->GetOrderMoveTagetSequence();
	if (MoveTargetIndex >= Sequence.Num())
	{
		if (Controller->GetLoopTargetSequence())
		{
			MoveTargetIndex = 0;
			bFirstLoop = false;
		}
		else
		{
			return EBTNodeResult::Succeeded;
		}
	}
	Controller->ReceiveMoveCompleted.AddDynamic(this, &UAMyTaskMoveTargetSequence::OnMoveCompleted);
	Controller->MoveToLocation(Sequence[MoveTargetIndex].Position);
	return EBTNodeResult::InProgress;
}

void UAMyTaskMoveTargetSequence::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	check(Controller != nullptr);
	auto Tree = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent());
	check(Tree != nullptr);
	if (Result != EPathFollowingResult::Success)
	{
		if (ensure(Tree != nullptr))
		{
			FinishLatentTask(*Tree, EBTNodeResult::Failed);
		}
		return;
	}

	Controller->ReceiveMoveCompleted.RemoveDynamic(this, &UAMyTaskMoveTargetSequence::OnMoveCompleted);
	const auto& MoveTarget = Controller->GetOrderMoveTagetSequence()[MoveTargetIndex];
	++MoveTargetIndex;

	UpdateBlackBoardKeyTargetIsMandatory();

	if (auto Pawn = Controller->GetPawn(); Pawn != nullptr)
	{
		Controller->SetFocalPoint(Pawn->GetActorLocation() + MoveTarget.Rotation.Vector());
	}

	auto Lambda =
		[this]
		{
			check(Controller != nullptr);
			Controller->ClearFocus(EAIFocusPriority::Gameplay);
			auto Tree = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent());
			check(Tree != nullptr);
			if (MoveTargetIndex == Controller->GetOrderMoveTagetSequence().Num() && !Controller->GetLoopTargetSequence())
			{
				FinishLatentTask(*Tree, EBTNodeResult::Succeeded);
			}
			else if (auto Result_ = Move(*Tree); Result_ != EBTNodeResult::InProgress)
			{
				FinishLatentTask(*Tree, Result_);
			}
		}
	;

	if (MoveTarget.TimeToStayInPosition > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, Lambda, MoveTarget.TimeToStayInPosition, /*bInLoop*/ false);
	}
	else
	{
		Lambda();
	}
}

void UAMyTaskMoveTargetSequence::UpdateBlackBoardKeyTargetIsMandatory()
{
	const auto Blackboard = Controller->GetBlackboardComponent();
	if (Blackboard != nullptr)
	{
		Blackboard->SetValueAsBool(
			Controller->MoveTargetIsMandatoryKeyName
			, MoveTargetIndex < Controller->GetFirstNonMandatoryOrderMoveTarget()
		);
	}
}
