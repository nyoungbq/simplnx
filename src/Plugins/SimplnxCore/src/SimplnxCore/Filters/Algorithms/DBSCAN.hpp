#pragma once

#include "SimplnxCore/SimplnxCore_export.hpp"

#include "simplnx/DataStructure/DataPath.hpp"
#include "simplnx/DataStructure/DataStructure.hpp"
#include "simplnx/Filter/IFilter.hpp"
#include "simplnx/Parameters/ArrayCreationParameter.hpp"
#include "simplnx/Parameters/ArraySelectionParameter.hpp"
#include "simplnx/Parameters/ChoicesParameter.hpp"
#include "simplnx/Parameters/NumberParameter.hpp"
#include "simplnx/Utilities/ClusteringUtilities.hpp"

#include <random>

namespace nx::core
{
struct SIMPLNXCORE_EXPORT DBSCANInputValues
{
  DataPath ClusteringArrayPath;
  DataPath MaskArrayPath;
  DataPath FeatureIdsArrayPath;
  float32 Epsilon;
  int32 MinPoints;
  ClusterUtilities::DistanceMetric DistanceMetric;
  DataPath FeatureAM;
  bool AllowCaching;
  bool UseRandom;
  std::mt19937_64::result_type Seed;
};

/**
 * @class
 */
class SIMPLNXCORE_EXPORT DBSCAN
{
public:
  DBSCAN(DataStructure& dataStructure, const IFilter::MessageHandler& mesgHandler, const std::atomic_bool& shouldCancel, DBSCANInputValues* inputValues);
  ~DBSCAN() noexcept;

  DBSCAN(const DBSCAN&) = delete;
  DBSCAN(DBSCAN&&) noexcept = delete;
  DBSCAN& operator=(const DBSCAN&) = delete;
  DBSCAN& operator=(DBSCAN&&) noexcept = delete;

  Result<> operator()();
  void updateProgress(const std::string& message);
  const std::atomic_bool& getCancel();

private:
  DataStructure& m_DataStructure;
  const DBSCANInputValues* m_InputValues = nullptr;
  const std::atomic_bool& m_ShouldCancel;
  const IFilter::MessageHandler& m_MessageHandler;
};

} // namespace nx::core
