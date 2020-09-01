// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generator.h"
#include "GameFramework/Actor.h"
#include "PropGeneratorItem.h"
#include "PropGenerator.generated.h"

USTRUCT(BlueprintType)
struct FProp
{
	GENERATED_BODY()

public:

	/** The prop to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APropGeneratorItem> Prop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator MinRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator MaxRotation;

	/** The maximum number of times this prop can be spawned */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnLimit;

private:

	/** The number of times the prop has been spawn in the current box */
	int32 SpawnCount;

public:

	FORCEINLINE void IncrementSpawnCount() { SpawnCount++; }

	FORCEINLINE bool SpawnCountUnderSpawnLimit() { return SpawnCount < SpawnLimit; }

};

USTRUCT(BlueprintType)
struct FPropGroup
{
	GENERATED_BODY()

	/** The tag used to find the box component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BoxComponentTag;

	/** The props to spawn and their parameters */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FProp> Props;

	/** The maximum number of props that will be spawned in the box component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxNumberOfPropsToSpawn;
};

UCLASS()
class DUNGEON_CPP_API APropGenerator : public AGenerator
{
	GENERATED_BODY()
	
public:	
	/** Sets default values for this actor's properties */
	APropGenerator();

	/** The props that will be spawned grouped by what box component they will spawn in */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Props")
	TArray<FPropGroup> PropGroups;

	/** The maximum number of times it will try to place a prop that is overlapping something else */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Props")
	int32 RetryLimit;

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

private:

	/** Try to spawn the actor in Prop with in Box, Actor won't be spawned if it's colliding with something */
	AActor* TryToSpawnPropInBox(FProp& Prop, class UBoxComponent* Box);

	/** Get a random rotator between the min and max values in Prop */
	FRotator GetRandomRotator(FProp Prop);

	/** Returns the first box component with the passed in Tag from BoxComponents */
	UBoxComponent* GetBoxComponentByTag(const TArray<UBoxComponent*>& BoxComponents, const FName Tag);
};
