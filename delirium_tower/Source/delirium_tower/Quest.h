// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "SM_State.h"
#include "Quest.generated.h"


UENUM()
enum class EQuestCompletion : uint8
{
	EQC_NotStarted,
	EQC_Started,
	EQC_Succeeded, 
	EQC_Failed
};

/**
 * 
 */
UCLASS()
class DELIRIUM_TOWER_API UQuest : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void OnSucceeded(class UQuestStatus* QuestStatus) const;
	virtual void OnFailed(class UQuestStatus* QuestStatus) const;
protected:
	// Quests that will start if we succeed this
	UPROPERTY(EditAnywhere)
	TArray<UQuest*> SuccessQuests;

	UPROPERTY(EditAnywhere)
	TArray<USM_InputAtom*> SuccessInputs;

	// Quests that will start if we fail this
	UPROPERTY(EditAnywhere)
	TArray<UQuest*> FailureQuests;

	// Input atoms to add to failure
	UPROPERTY(EditAnywhere)
	TArray<USM_InputAtom*> FailureInputs;
};

UCLASS()
class DELIRIUM_TOWER_API UQuestWithResult : public UDataAsset
{
	GENERATED_BODY()

public:
	// Name of the quest
	UPROPERTY(EditAnywhere)
		FText QuestName;

	// If this quest accepts our quest activities log, it's succesful
	UPROPERTY(EditAnywhere)
		USM_State* QuestStateMachine;

	// If true, the inputlist is a blacklist
	UPROPERTY(EditAnywhere)
		uint32 bInputBlackList : 1;

	// The blacklist used to filter InputAtoms this quest recognizes
	UPROPERTY(EditAnywhere)
		TArray<USM_InputAtom*> InputList;

	virtual void OnSucceeded(class UQuestStatus* QuestStatus) const;
	virtual void OnFailed(class UQuestStatus* QuestStatus) const;
};

USTRUCT()
struct FQuestInProgress
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	const UQuest * Quest;

	UPROPERTY(EditAnywhere)
	EQuestCompletion QuestProgress;

protected:

	UPROPERTY(EditAnywhere)
	TArray<USM_InputAtom*> QuestActivities;

public:

	// Returns true if the quest was completed (success or failure) by the new activity.
	bool UpdateQuest(const UObject* ObjectRef, USM_InputAtom* QuestActivity)
	{
		// Only log activity to valid, in-progress quests. Check the blacklists before logging
		if (Quest && (QuestProgress == EQuestCompletion::EQC_Started) 
		&& (Quest->bInputBlackList != Quest->InputList.Contains(QuestActivity)) )
		{
			FStateMachineResult QuestResult;
			QuestActivities.Add(QuestActivity);
			QuestResult = Quest->QuestStateMachine->RunState(ObjectRef, QuestActivities);
			switch (QuestResult.CompletionType)
			{
			case EStateMachineCompletionType::Accepted:
				QuestProgress = EQuestCompletion::EQC_Succeeded;
				return true;

			case EStateMachineCompletionType::Rejected:
				QuestProgress = EQuestCompletion::EQC_Failed;
				return true;
			//case EStateMachineCompletionType::NotAccepted: // Still in progress, no update needed/
			
			}
		}
		return false;
	}

	static FQuestInProgress NewQuestInProgress(const UQuest* _Quest)
	{
		FQuestInProgress NewQIP;
		NewQIP.Quest = _Quest;
		NewQIP.QuestProgress = EQuestCompletion::EQC_Started;
		return NewQIP;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )

class DELIRIUM_TOWER_API UQuestStatus : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets defaultValue for this component's properties
	UQuestStatus();
	
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	/* Add to our quest activity log! This also automatically checks to see if any 
	unfinished quests are now complete */
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void UpdateQuests(USM_InputAtom* QuestActivity);

	/*
		Add a new quest-in-progress entry, or begin the quest provided if it's already on the list and hasn't been started yet.
	*/
	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool BeginQuest(const UQuest* Quest);
protected:
	// The master list of all quests related things we done
	UPROPERTY(EditAnywhere)
	TArray<USM_InputAtom*> QuestActivities;
	
	// The list of quests in our current game or area
	UPROPERTY(EditAnywhere)
	TArray<FQuestInProgress> QuestList;


};


