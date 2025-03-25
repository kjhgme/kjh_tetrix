// Fill out your copyright notice in the Description page of Project Settings.


#include "BGMaker.h"

// Sets default values
ABGMaker::ABGMaker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABGMaker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABGMaker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABGMaker::MapSetting(int x, int y)
{
	if (bHasBeenSet == true)
	{
		for (AActor* Block : SpawnedBlocks)
		{
			if (Block)
			{
				Block->Destroy();
			}
		}
		SpawnedBlocks.Empty();
	}	

	float StartY = -(x - 1) * 25.0f;
	FVector Loc = { -50.0f, StartY, 25.0f };

	for (int i = 0; i < y; ++i)
	{
		Loc.Y = StartY;

		for (int j = 0; j < x; ++j)
		{
			AActor* BGBlock = GetWorld()->SpawnActor<AActor>(BGActorClass);
			if (!BGBlock) continue;

			BGBlock->SetActorLocation(Loc);
			SpawnedBlocks.Add(BGBlock);
			Loc.Y += 50.0f;
		}

		Loc.Z += 50.0f;
	}

	bHasBeenSet = true;
}