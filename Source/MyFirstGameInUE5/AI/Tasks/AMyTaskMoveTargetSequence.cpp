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

	auto& SequenceData = Controller->GetMoveTargetSequenceTaskData();
	const auto& Sequence = SequenceData.OrderMoveTagetSequence;
	if (SequenceData.CurrentMoveTargetIndex >= Sequence.Num())
	{
		if (SequenceData.bLoopTargetSequence)
		{
			SequenceData.CurrentMoveTargetIndex = 0;
			SequenceData.bCurrentLoopIsFirst = false;
		}
		else
		{
			return EBTNodeResult::Succeeded;
		}
	}
	Controller->ReceiveMoveCompleted.AddDynamic(this, &UAMyTaskMoveTargetSequence::OnMoveCompleted);
	Controller->MoveToLocation(Sequence[SequenceData.CurrentMoveTargetIndex].Position);
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
	auto& SequenceData = Controller->GetMoveTargetSequenceTaskData();
	const auto& MoveTarget = SequenceData.OrderMoveTagetSequence[SequenceData.CurrentMoveTargetIndex];
	++SequenceData.CurrentMoveTargetIndex;

	UpdateBlackBoardKeyTargetIsMandatory();

	if (auto Pawn = Controller->GetPawn(); Pawn != nullptr)
	{
		Controller->SetFocalPoint(Pawn->GetActorLocation() + MoveTarget.Rotation.Vector());
	}

	if (MoveTarget.TimeToStayInPosition > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle
			, this
			, &UAMyTaskMoveTargetSequence::StayingInPosotionFinished
			, MoveTarget.TimeToStayInPosition
			, /*bInLoop*/ false
		);
	}
	else
	{
		StayingInPosotionFinished();
	}
}

void UAMyTaskMoveTargetSequence::StayingInPosotionFinished()
{
	check(Controller != nullptr);
	Controller->ClearFocus(EAIFocusPriority::Gameplay);
	auto Tree = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent());
	check(Tree != nullptr);
	auto& SequenceData = Controller->GetMoveTargetSequenceTaskData();
	if (SequenceData.CurrentMoveTargetIndex == SequenceData.OrderMoveTagetSequence.Num() && !SequenceData.bLoopTargetSequence)
	{
		FinishLatentTask(*Tree, EBTNodeResult::Succeeded);
	}
	else if (auto Result_ = Move(*Tree); Result_ != EBTNodeResult::InProgress)
	{
		FinishLatentTask(*Tree, Result_);
	}
}

void UAMyTaskMoveTargetSequence::UpdateBlackBoardKeyTargetIsMandatory()
{
	const auto Blackboard = Controller->GetBlackboardComponent();
	if (Blackboard != nullptr)
	{
		auto& SequenceData = Controller->GetMoveTargetSequenceTaskData();
		Blackboard->SetValueAsBool(
			Controller->MoveTargetIsMandatoryKeyName
			, SequenceData.CurrentMoveTargetIndex < SequenceData.FirstNonMandatoryOrderMoveTarget
		);
	}
}
