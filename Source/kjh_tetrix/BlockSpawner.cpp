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
    ZCount.SetNum(GridSizeX);

    CheckAndClearLines();

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
            const float NewY = ComponentLocation.Y - 50.0f;

            if (ComponentLocation.Y < -(GridSizeX - 2) * 25.0f)
            {
                return;
            }

            for (AActor* PlacedBlock : PlacedBlocks)
            {
                if (!PlacedBlock) continue;

                TArray<UActorComponent*> PlacedComponents;
                PlacedBlock->GetComponents(PlacedComponents);

                for (UActorComponent* PlacedComponent : PlacedComponents)
                {
                    USceneComponent* PlacedSceneComponent = Cast<USceneComponent>(PlacedComponent);
                    if (!PlacedSceneComponent) continue;

                    FVector PlacedLocation = PlacedSceneComponent->GetComponentLocation();

                    if (FMath::IsNearlyEqual(ComponentLocation.Z, PlacedLocation.Z, 0.1f) &&
                        FMath::IsNearlyEqual(NewY, PlacedLocation.Y, 0.1f))
                    {
                        return;
                    }
                }
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
            const float NewY = ComponentLocation.Y + 50.0f;

            if (ComponentLocation.Y > (GridSizeX - 2) * 25.0f)
            {
                return;
            }

            for (AActor* PlacedBlock : PlacedBlocks)
            {
                if (!PlacedBlock) continue;

                TArray<UActorComponent*> PlacedComponents;
                PlacedBlock->GetComponents(PlacedComponents);

                for (UActorComponent* PlacedComponent : PlacedComponents)
                {
                    USceneComponent* PlacedSceneComponent = Cast<USceneComponent>(PlacedComponent);
                    if (!PlacedSceneComponent) continue;

                    FVector PlacedLocation = PlacedSceneComponent->GetComponentLocation();

                    if (FMath::IsNearlyEqual(ComponentLocation.Z, PlacedLocation.Z, 0.1f) &&
                        FMath::IsNearlyEqual(NewY, PlacedLocation.Y, 0.1f))
                    {
                        return;
                    }
                }
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
        // 바닥에 닿을 때
        USceneComponent* SceneComponent = Cast<USceneComponent>(Component);
        if (SceneComponent)
        {
            FVector ComponentLocation = SceneComponent->GetComponentLocation();
            float NewZ = ComponentLocation.Z - 50.0f;

            if (ComponentLocation.Z <= 30.0f)   // 25.0f
            {
                SpawnedBlock->SetActorEnableCollision(true);
                PlacedBlocks.Add(SpawnedBlock);
                SpawnedBlock = nullptr;

                SpawnBlock();

                return;
            }

            // 아래에 블록이 있을 때
            for (AActor* PlacedBlock : PlacedBlocks)
            {
                if (!PlacedBlock) continue;

                TArray<UActorComponent*> PlacedComponents;
                PlacedBlock->GetComponents(PlacedComponents);

                for (UActorComponent* PlacedComponent : PlacedComponents)
                {
                    USceneComponent* PlacedSceneComponent = Cast<USceneComponent>(PlacedComponent);
                    if (!PlacedSceneComponent) continue;

                    FVector PlacedComponentLocation = PlacedSceneComponent->GetComponentLocation();
                    
                    bool bSameX = FMath::IsNearlyEqual(ComponentLocation.X, PlacedComponentLocation.X, 0.1f);
                    bool bSameY = FMath::IsNearlyEqual(ComponentLocation.Y, PlacedComponentLocation.Y, 0.1f);

                    float SpawnedMinZ = NewZ - 25.0f;
                    float SpawnedMaxZ = NewZ + 25.0f;
                    float PlacedMinZ = PlacedComponentLocation.Z - 25.0f;
                    float PlacedMaxZ = PlacedComponentLocation.Z + 25.0f;

                    if (bSameX && bSameY && (SpawnedMaxZ > PlacedMinZ) && (SpawnedMinZ < PlacedMaxZ))
                    {
                        SpawnedBlock->SetActorEnableCollision(true);
                        PlacedBlocks.Add(SpawnedBlock);
                        SpawnedBlock = nullptr;

                        SpawnBlock();
                        return;
                    }
                }
            }
        }        
    }

    SpawnedBlock->AddActorWorldOffset(MoveOffset);
}

void ABlockSpawner::MoveToBottom()
{
}

void ABlockSpawner::Rotate()
{
    if (!SpawnedBlock) return;

    // 1. 초기 상태 저장
    const FRotator OriginalRotation = SpawnedBlock->GetActorRotation();
    const FVector OriginalLocation = SpawnedBlock->GetActorLocation();

    // 2. 회전 실행
    SpawnedBlock->SetActorRotation(OriginalRotation + FRotator(0, 0, 90));

    // 3. 충돌 검사 및 조정
    FVector TotalAdjustment(0);
    bool bNeedMoreCheck = true;
    int32 SafetyCounter = 0;

    while (bNeedMoreCheck && SafetyCounter++ < 10)
    {
        bNeedMoreCheck = false;
        FVector RequiredAdjustment(0);

        // 모든 컴포넌트에 대한 충돌 검사
        TArray<UActorComponent*> Components;
        SpawnedBlock->GetComponents(Components);

        for (UActorComponent* Component : Components)
        {
            USceneComponent* SceneComp = Cast<USceneComponent>(Component);
            if (!SceneComp) continue;

            FVector CompLocation = SceneComp->GetComponentLocation();

            // 경계 영역 검사
            const float MinY = -(GridSizeX - 1) * 25.0f;
            const float MaxY = GridSizeX * 25.0f;
            const float MinZ = 25.0f;

            // 왼쪽 경계
            if (CompLocation.Y < MinY)
            {
                RequiredAdjustment.Y += 50.0f;
                bNeedMoreCheck = true;
            }
            // 오른쪽 경계
            else if (CompLocation.Y > MaxY)
            {
                RequiredAdjustment.Y -= 50.0f;
                bNeedMoreCheck = true;
            }

            // 바닥 경계
            if (CompLocation.Z < MinZ)
            {
                RequiredAdjustment.Z += 50.0f;
                bNeedMoreCheck = true;
            }

            // 다른 블록과의 충돌 검사
            for (AActor* PlacedBlock : PlacedBlocks)
            {
                if (!PlacedBlock) continue;

                TArray<UActorComponent*> PlacedComponents;
                PlacedBlock->GetComponents(PlacedComponents);

                for (UActorComponent* PlacedComponent : PlacedComponents)
                {
                    USceneComponent* PlacedSceneComp = Cast<USceneComponent>(PlacedComponent);
                    if (!PlacedSceneComp) continue;

                    FVector PlacedLocation = PlacedSceneComp->GetComponentLocation();

                    // X축 정렬 확인
                    if (!FMath::IsNearlyEqual(CompLocation.X, PlacedLocation.X, 1.0f))
                        continue;

                    // Y축 겹침 검사
                    const float BlockHalfSize = 25.0f;
                    const float ThisMinY = CompLocation.Y - BlockHalfSize;
                    const float ThisMaxY = CompLocation.Y + BlockHalfSize;
                    const float PlacedMinY = PlacedLocation.Y - BlockHalfSize;
                    const float PlacedMaxY = PlacedLocation.Y + BlockHalfSize;

                    const bool bYOverlap = (ThisMaxY > PlacedMinY) && (ThisMinY < PlacedMaxY);

                    // Z축 겹침 검사
                    const float ThisMinZ = CompLocation.Z - BlockHalfSize;
                    const float ThisMaxZ = CompLocation.Z + BlockHalfSize;
                    const float PlacedMinZ = PlacedLocation.Z - BlockHalfSize;
                    const float PlacedMaxZ = PlacedLocation.Z + BlockHalfSize;

                    const bool bZOverlap = (ThisMaxZ > PlacedMinZ) && (ThisMinZ < PlacedMaxZ);

                    // 충돌 발생 시 Y축 조정
                    if (bYOverlap && bZOverlap)
                    {
                        const float OverlapY = CompLocation.Y - PlacedLocation.Y;
                        RequiredAdjustment.Y += (OverlapY > 0) ? 50.0f : -50.0f;
                        bNeedMoreCheck = true;
                    }
                }
            }
        }

        // 조정 적용
        if (bNeedMoreCheck)
        {
            SpawnedBlock->AddActorWorldOffset(RequiredAdjustment);
            TotalAdjustment += RequiredAdjustment;
        }
    }

    // 4. 최종 검증
    if (IsCollidingWithAnything())
    {
        // 충돌 시 원상태 복구
        SpawnedBlock->SetActorRotation(OriginalRotation);
        SpawnedBlock->SetActorLocation(OriginalLocation);
    }
}

bool ABlockSpawner::IsCollidingWithAnything() const
{
    if (!SpawnedBlock) return false;

    const float BlockHalfSize = 25.0f;
    const float MinYBoundary = -(GridSizeX - 1) * 25.0f;
    const float MaxYBoundary = GridSizeX * 25.0f;
    const float MinZBoundary = 25.0f;

    TArray<UActorComponent*> Components;
    SpawnedBlock->GetComponents(Components);

    USceneComponent* RootComp = SpawnedBlock->GetRootComponent();

    // 모든 컴포넌트 검사
    for (UActorComponent* Component : Components)
    {
        USceneComponent* SceneComp = Cast<USceneComponent>(Component);
        if (!SceneComp || SceneComp == RootComp) continue;

        const FVector CompLocation = SceneComp->GetComponentLocation();

        // 1. 경계 영역 충돌 검사
        if (CompLocation.Y < MinYBoundary || CompLocation.Y > MaxYBoundary)
            return true;

        if (CompLocation.Z < MinZBoundary)
            return true;

        // 2. 다른 블록과의 충돌 검사
        for (AActor* PlacedBlock : PlacedBlocks)
        {
            if (!PlacedBlock) continue;

            TArray<UActorComponent*> PlacedComponents;
            PlacedBlock->GetComponents(PlacedComponents);

            for (UActorComponent* PlacedComponent : PlacedComponents)
            {
                USceneComponent* PlacedSceneComp = Cast<USceneComponent>(PlacedComponent);
                if (!PlacedSceneComp) continue;

                const FVector PlacedLocation = PlacedSceneComp->GetComponentLocation();

                // X축 정렬 확인 (같은 열인지)
                if (!FMath::IsNearlyEqual(CompLocation.X, PlacedLocation.X, 1.0f))
                    continue;

                // Y축 겹침 검사
                const float ThisMinY = CompLocation.Y - BlockHalfSize;
                const float ThisMaxY = CompLocation.Y + BlockHalfSize;
                const float PlacedMinY = PlacedLocation.Y - BlockHalfSize;
                const float PlacedMaxY = PlacedLocation.Y + BlockHalfSize;

                const bool bYOverlap = (ThisMaxY > PlacedMinY) && (ThisMinY < PlacedMaxY);

                // Z축 겹침 검사
                const float ThisMinZ = CompLocation.Z - BlockHalfSize;
                const float ThisMaxZ = CompLocation.Z + BlockHalfSize;
                const float PlacedMinZ = PlacedLocation.Z - BlockHalfSize;
                const float PlacedMaxZ = PlacedLocation.Z + BlockHalfSize;

                const bool bZOverlap = (ThisMaxZ > PlacedMinZ) && (ThisMinZ < PlacedMaxZ);

                // 3D 공간에서의 완전한 충돌 확인
                if (bYOverlap && bZOverlap)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void ABlockSpawner::CheckAndClearLines()
{
    ZCount.Init(0, GridSizeX);

    auto GetZIndex = [&](float Z)
    {
        return FMath::Clamp(
            FMath::FloorToInt(Z / 50.0f),
            0,
            GridSizeX - 1
        );
    };

    for (AActor* Block : PlacedBlocks)
    {
        if (!Block) continue;

        TArray<USceneComponent*> PlacedComponents;
        Block->GetComponents(PlacedComponents);

        USceneComponent* RootComp = Block->GetRootComponent();

        for (UActorComponent* Comp : PlacedComponents)
        {
            USceneComponent* SceneComp = Cast<USceneComponent>(Comp);
            if (!SceneComp || SceneComp == RootComp) continue;

            const int32 ZIdx = GetZIndex(SceneComp->GetComponentLocation().Z);
            ZCount[ZIdx]++;
        }
    }

    for (int32 i = 0; i < GridSizeX; ++i)
    {
        if (ZCount[i] >= GridSizeX)
        {
            for (AActor* Block : PlacedBlocks)
            {
                if (!Block) continue;

                TArray<UActorComponent*> PlacedComponents;
                Block->GetComponents(PlacedComponents);

                for (UActorComponent* Comp : PlacedComponents)
                {
                    USceneComponent* SceneComp = Cast<USceneComponent>(Comp);
                    if (!SceneComp) continue;

                    int num = static_cast<int>((SceneComp->GetComponentLocation().Z - 25.0f) / 50.0f);

                    if (num == i)
                    {
                        SceneComp->DestroyComponent();
                    }
                }
            }
        }
    }

    for (int i = 0; i < GridSizeX; ++i)
    {
        if (ZCount[i] >= GridSizeX)
        {
            for (AActor* Block : PlacedBlocks)
            {
                if (!Block) continue;

                TArray<UActorComponent*> PlacedComponents;
                Block->GetComponents(PlacedComponents);

                for (UActorComponent* Comp : PlacedComponents)
                {
                    USceneComponent* SceneComp = Cast<USceneComponent>(Comp);
                    if (!SceneComp) continue;

                    int num = static_cast<int>((SceneComp->GetComponentLocation().Z - 25.0f) / 50.0f);

                    if (num > i)
                    {
                        FTransform TransformOffset(FRotator::ZeroRotator, FVector(0.0f, 0.0f, -50.0f));
                        SceneComp->AddWorldTransform(TransformOffset);
                    }
                }
            }
        }
    }
}