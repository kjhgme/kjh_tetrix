// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockSpawner.h"
#include "TetrixPlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ABlockSpawner::ABlockSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlockSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlockSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlockSpawner::SpawnBlock()
{
    if (Blocks.Num() == 0) return;

    int32 RandomIndex = FMath::RandRange(0, Blocks.Num() - 1);
    TSubclassOf<AActor> SelectedBlockClass = Blocks[RandomIndex];

    if (!SelectedBlockClass) return; 

    float StartY;
    if (GridSizeX % 2 == 0)
        StartY = -25.0f;
    else
        StartY = 0.0f;

    FVector SpawnLocation = { -100.0f, StartY, 25.0f + 50.0f * (GridSizeY - 1) };
    FRotator SpawnRotation = FRotator::ZeroRotator;

    SpawnedBlock = GetWorld()->SpawnActor<AActor>(SelectedBlockClass, SpawnLocation, SpawnRotation);

    if (SpawnedBlock)
    {
        UE_LOG(LogTemp, Warning, TEXT("Block Spawned: %s"), *SpawnedBlock->GetName());
    }
}

void ABlockSpawner::MoveLeft()
{
    TArray<UActorComponent*> Components;
    SpawnedBlock->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
        if (SceneComponent)
        {
            FVector ComponentLocation = SceneComponent->GetComponentLocation();

            if (ComponentLocation.Y <= -(GridSizeX - 1) * 25.0f)
            {              
                return;
            }
        }
    }

    FTransform TransformOffset(FRotator::ZeroRotator, FVector(0.0f, -50.0f, 0.0f));
    SpawnedBlock->AddActorWorldTransform(TransformOffset);
}

void ABlockSpawner::MoveRight()
{
    TArray<UActorComponent*> Components;
    SpawnedBlock->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
        if (SceneComponent)
        {
            FVector ComponentLocation = SceneComponent->GetComponentLocation();

            if (ComponentLocation.Y >= (GridSizeX - 1) * 25.0f)
            {
                return;
            }
        }
    }

    FTransform TransformOffset(FRotator::ZeroRotator, FVector(0.0f, 50.0f, 0.0f));
    SpawnedBlock->AddActorWorldTransform(TransformOffset);
}

void ABlockSpawner::MoveDown()
{
    if (!SpawnedBlock) return;

    FVector MoveOffset(0.0f, 0.0f, -50.0f);
    FVector NewLocation = SpawnedBlock->GetActorLocation() + MoveOffset;

    TArray<UActorComponent*> Components;
    SpawnedBlock->GetComponents(Components);

    for (UActorComponent* Component : Components)
    {
        USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
        if (SceneComponent)
        {
            FVector ComponentLocation = SceneComponent->GetComponentLocation();

            if (ComponentLocation.Z <= 25.0f)
            {
                SpawnedBlock->SetActorEnableCollision(true);
                PlacedBlocks.Add(SpawnedBlock);
                SpawnedBlock = nullptr;

                SpawnBlock();

                return;
            }
        }

       /* for (AActor* PlacedBlock : PlacedBlocks)
        {
            if (!PlacedBlock) continue;

            TArray<UActorComponent*> PlacedComponents;
            PlacedBlock->GetComponents(PlacedComponents);

            for (UActorComponent* PlacedComponent : PlacedComponents)
            {
                USceneComponent* PlacedSceneComponent = Cast<USceneComponent>(PlacedComponent);
                if (!PlacedSceneComponent) continue;

                FVector PlacedComponentLocation = PlacedSceneComponent->GetComponentLocation();

                if (NewLocation.Z <= PlacedComponentLocation.Z)
                {
                    SpawnedBlock->SetActorEnableCollision(true);
                    PlacedBlocks.Add(SpawnedBlock);
                    SpawnedBlock = nullptr;

                    SpawnBlock();

                    return;
                }
            }
        }*/
    }

    SpawnedBlock->AddActorWorldOffset(MoveOffset);
}

void ABlockSpawner::MoveToBottom()
{
}
