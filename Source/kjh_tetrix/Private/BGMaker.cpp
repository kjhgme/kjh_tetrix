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
	for (int i = 0; i < x; ++i)
	{
		for (int j = 0; j < y; ++j)
		{
			GetWorld()->SpawnActor<AActor>(BGActorClass);
		}
	}
}

