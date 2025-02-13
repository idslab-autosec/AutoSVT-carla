// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once


#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Sensor/LidarDescription.h"
#include "Carla/Sensor/Sensor.h"
#include "Carla/Sensor/RayCastSemanticLidar.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"

#include <compiler/disable-ue4-macros.h>
#include <carla/sensor/data/LidarData.h>
#include <compiler/enable-ue4-macros.h>

#include "RayCastLidarWithFog.generated.h"

struct FogDensityDataPoint {
	float x;
	float y;
};

/// A ray-cast based Lidar sensor.
UCLASS()
class CARLA_API ARayCastLidarWithFog : public ARayCastSemanticLidar
{
	GENERATED_BODY()

	using FLidarData = carla::sensor::data::LidarWithFogData;
	using FDetection = carla::sensor::data::LidarWithFogDetection;

public:
	static FActorDefinition GetSensorDefinition();

	ARayCastLidarWithFog(const FObjectInitializer& ObjectInitializer);
	virtual void Set(const FActorDescription& Description) override;
	virtual void Set(const FLidarDescription& LidarDescription) override;

	virtual void PostPhysTick(UWorld* World, ELevelTick TickType, float DeltaTime);

private:
	/// Compute the received intensity of the point
	float ComputeIntensity(const FSemanticDetection& RawDetection) const;
	FDetection ComputeDetection(const FHitResult& HitInfo, const FTransform& SensorTransf);

	void PreprocessRays(uint32_t Channels, uint32_t MaxPointsPerChannel) override;
	bool PostprocessDetection(FDetection& Detection) const;

	void ComputeAndSaveDetections(const FTransform& SensorTransform) override;

	std::vector<std::string> SplitString(const std::string& Input, char Delimiter) const;

	void GetStepSizeData(float FogDensity) const;

	std::string GetAlphaInFileName(float FogDensity) const;

	float PiecewiseLinearRegression(const std::vector<FogDensityDataPoint>& data, float x) const;

	float CalculateMOR(const float FogDensity) const;

	float GetReflectivity(const uint32 ObjectTag) const;

	float LookupReflectivityTable(FString ActorLabel) const;

	std::string GetPathSeparator() const;

	FLidarData LidarData;

	/// Enable/Disable general dropoff of lidar points
	bool DropOffGenActive;

	/// Slope for the intensity dropoff of lidar points, it is calculated
	/// throught the dropoff limit and the dropoff at zero intensity
	/// The points is kept with a probality alpha*Intensity + beta where
	/// alpha = (1 - dropoff_zero_intensity) / droppoff_limit
	/// beta = (1 - dropoff_zero_intensity)
	float DropOffAlpha;
	float DropOffBeta;

	// mutable std::string CacheAlpha;
	float CurrentFogDensity;
	mutable std::map<std::string, std::vector<std::string>> StepSizeData;

	// Fog Simulation Parameter
	float Mor = 10000;
	float Alpha = 0.0f;
	float Beta;
};
