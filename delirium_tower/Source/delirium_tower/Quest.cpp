// Fill out your copyright notice in the Description page of Project Settings.

#include "delirium_tower.h"
#include "Quest.h"


void UQuest::OnSucceeded(class UQuestStatus* QuestStatus) const
{
	UE_LOG(LogTemp, Warning, TEXT("Quest \"%s\" succeeded!"), *QuestName.ToString());
}
void UQuest::OnFailed(class UQuestStatus* QuestStatus) const
{
	UE_LOG(LogTemp, Warning, TEXT("Quest \"%s\" failed!"), *QuestName.ToString());
}

void UQuestWithResult::OnSucceeded(class UQuestStatus* QuestStatus) const
{
	Super::OnSucceeded(QuestStatus);
	for (UQuest* SuccessQuest : SuccessQuests)
	{
		QuestStatus->BeginQuest(SuccessQuest);
	}

	for (int32 i = 0; i < SuccessInputs.Num(); ++i)
	{
		QuestStatus->UpdateQuests(SuccessInputs[i]);
	}
}
void UQuestWithResult::OnFailed(class UQuestStatus* QuestStatus) const
{
//	UE_LOG(LogTemp, Warning, TEXT("Quest \"%s\" failed!"), *QuestName.ToString());
	Super::OnSucceeded(QuestStatus);
	for (UQuest* FailureQuest : FailureQuests)
	{
		QuestStatus->BeginQuest(FailureQuest);
	}

	for (int32 i = 0; i < FailureInputs.Num(); ++i)
	{
		QuestStatus->UpdateQuests(FailureInputs[i]);
	}
}
UQuestStatus::UQuestStatus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

}

void UQuestStatus::BeginPlay()
{
	Super::BeginPlay();
}

void UQuestStatus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UQuestStatus::UpdateQuests(USM_InputAtom* QuestActivity)
{

	TArray<int32> RecentlyCompletedQuests;
	// Update the master list  of everything we've ever done
	QuestActivities.Add(QuestActivity);

	// Update individual quests 
	for (int32 i = QuestList.Num() - 1; i >= 0; --i)
	{
		if (QuestList[i].UpdateQuest(this, QuestActivity))
		{
			RecentlyCompletedQuests.Add(i);
		}
	}

	for (int32 i = RecentlyCompletedQuests.Num() - 1; i >= 0; --i)
	{
		FQuestInProgress& QIP = QuestList[RecentlyCompletedQuests[i]];
		if (QIP.QuestProgress == EQuestCompletion::EQC_Succeeded)
		{
			QIP.Quest->OnSucceeded(this);
		}
		else
		{
			QIP.Quest->OnFailed(this);
		}
		RecentlyCompletedQuests.RemoveAtSwap(i);

	}
}


bool UQuestStatus::BeginQuest(const UQuest* Quest)
{
	for (FQuestInProgress& QIP : QuestList)
	{
		if (QIP.Quest == Quest)
		{
			if (QIP.QuestProgress == EQuestCompletion::EQC_NotStarted)
			{
				UE_LOG(LogTemp, Warning, TEXT("Changing Quest \"%s\" to Started status"), *QIP.Quest->QuestName.ToString());
				QIP.QuestProgress = EQuestCompletion::EQC_Started; 
				return true;
			}

			UE_LOG(LogTemp, Warning, TEXT("Quest \"%s\" is Already in the list / not correct state"), *QIP.Quest->QuestName.ToString());
			return false;
		}

	}

	QuestList.Add(FQuestInProgress::NewQuestInProgress(Quest));
	return false;
}




