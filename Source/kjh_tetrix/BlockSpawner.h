// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockSpawner.generated.h"

UCLASS()
class KJH_TETRIX_API ABlockSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlockSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> Blocks;

	UPROPERTY(BlueprintReadWrite)
	AActor* SpawnedBlock;

	TArray<AActor*> PlacedBlocks;
	
	TArray<USceneComponent*> AllComponents;
	TArray<AActor*> ValidBlocks;

	UFUNCTION(BlueprintCallable)
	void SpawnBlock();

	UFUNCTION(BlueprintCallable)
	void MoveLeft();

	UFUNCTION(BlueprintCallable)
	void MoveRight();

	UFUNCTION(BlueprintCallable)
	void MoveDown();

	UFUNCTION(BlueprintCallable)
	void MoveToBottom();

	UFUNCTION(BlueprintCallable)
	void Rotate();

	void CheckAndClearLines();

	UFUNCTION(BlueprintCallable)
	void GetMapSize(int GridX, int GridY)
	{
		GridSizeX = GridX;
		GridSizeY = GridY;
	}

	int GridSizeX;
	int GridSizeY;

};
