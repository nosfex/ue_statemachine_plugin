// Fill out your copyright notice in the Description page of Project Settings.

#include "delirium_tower.h"
#include "Quest.h"

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
	// Update the master list  of everything we've ever done
	QuestActivities.Add(QuestActivity);

	// Update individual quests 
	for (int32 i = QuestList.Num() - 1; i >= 0; --i)
	{
		if (QuestList[i].UpdateQuest(this, QuestActivity))
		{
			UE_LOG(LogTemp, Warning, TEXT("Quest \"%s\" completed: %s"), *QuestList[i].Quest->QuestName.ToString(), (QuestList[i].QuestProgress == EQuestCompletion::EQC_Succeeded) ? TEXT("Succeeded") : TEXT("Failed"));
		}
	}
}



