// Fill out your copyright notice in the Description page of Project Settings.


#include "PropGenerator.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APropGenerator::APropGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APropGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	// Get all BoxComponents in the Actor
	TArray<UBoxComponent*> BoxComponents;
	GetComponents<UBoxComponent>(BoxComponents);

	for (FPropGroup PropGroup : PropGroups)
	{		
		// Get the box compoenent with the same tag at the prop group
		UBoxComponent* BoxToSpawnIn = GetBoxComponentByTag(BoxComponents, PropGroup.BoxComponentTag);

		if (BoxToSpawnIn)
		{
			int32 PropsSpawnedCount = 0;
			int32 NumberOfRetries = 0;

			while (PropsSpawnedCount < PropGroup.MaxNumberOfPropsToSpawn && NumberOfRetries < RetryLimit)
			{
				int32 PropToSpawnIndex = UKismetMathLibrary::RandomIntegerInRange(0, PropGroup.Props.Num() - 1);
				FProp& PropToSpawn = PropGroup.Props[PropToSpawnIndex];

				if (PropToSpawn.SpawnCountUnderSpawnLimit())
				{
					AActor* SpawnedActor = TryToSpawnPropInBox(PropToSpawn, BoxToSpawnIn);

					if (SpawnedActor)
					{
						PropToSpawn.IncrementSpawnCount();
						PropsSpawnedCount++;
						NumberOfRetries = 0;
					}
					else
					{
						NumberOfRetries++;
					}
				}
				else
				{
					NumberOfRetries++;
				}
			}
		}
	}
}

AActor* APropGenerator::TryToSpawnPropInBox(FProp &Prop, UBoxComponent* Box)
{
	FVector BoxSize = Box->GetScaledBoxExtent();
		
	FVector PointInBox = UKismetMathLibrary::RandomPointInBoundingBox(FVector(0.f), BoxSize);
	FVector SpawnLocation = UKismetMathLibrary::TransformLocation(Box->GetComponentTransform(), PointInBox);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	return GetWorld()->SpawnActor<AActor>(Prop.Prop, SpawnLocation, GetRandomRotator(Prop), SpawnParams);
}

FRotator APropGenerator::GetRandomRotator(FProp Prop)
{
	float Pitch = UKismetMathLibrary::RandomFloatInRange(Prop.MinRotation.Pitch, Prop.MaxRotation.Pitch);
	float Yaw = UKismetMathLibrary::RandomFloatInRange(Prop.MinRotation.Yaw, Prop.MaxRotation.Yaw);
	float Roll = UKismetMathLibrary::RandomFloatInRange(Prop.MinRotation.Roll, Prop.MaxRotation.Roll);

	return FRotator(Pitch, Yaw, Roll);
}

UBoxComponent* APropGenerator::GetBoxComponentByTag(const TArray<UBoxComponent*> &BoxComponents, const FName Tag)
{
	for (UBoxComponent* Box : BoxComponents)
	{
		if (Box->ComponentHasTag(Tag))
			return Box;
	}

	return nullptr;
}