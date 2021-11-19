#include "GenerateEnsembleStatistics.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/EmptyAction.hpp"
#include "complex/Parameters/ArrayCreationParameter.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/PhaseTypeSelectionFilterParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string GenerateEnsembleStatistics::name() const
{
  return FilterTraits<GenerateEnsembleStatistics>::name.str();
}

//------------------------------------------------------------------------------
std::string GenerateEnsembleStatistics::className() const
{
  return FilterTraits<GenerateEnsembleStatistics>::className;
}

//------------------------------------------------------------------------------
Uuid GenerateEnsembleStatistics::uuid() const
{
  return FilterTraits<GenerateEnsembleStatistics>::uuid;
}

//------------------------------------------------------------------------------
std::string GenerateEnsembleStatistics::humanName() const
{
  return "Generate Ensemble Statistics";
}

//------------------------------------------------------------------------------
std::vector<std::string> GenerateEnsembleStatistics::defaultTags() const
{
  return {"#Statistics", "#Ensemble"};
}

//------------------------------------------------------------------------------
Parameters GenerateEnsembleStatistics::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  /*[x]*/ params.insert(std::make_unique<PhaseTypeSelectionFilterParameter>(k_PhaseTypeArray_Key, "Phase Types", "", {}));
  params.insert(std::make_unique<Float32Parameter>(k_SizeCorrelationResolution_Key, "Size Correlation Spacing", "", 1.23345f));
  params.insertSeparator(Parameters::Separator{"Cell Feature Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeaturePhasesArrayPath_Key, "Feature Phases", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_NeighborListArrayPath_Key, "Neighbor List", "", DataPath{}));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_CalculateMorphologicalStats_Key, "Calculate Morphological Statistics", "", false));
  params.insert(std::make_unique<ChoicesParameter>(k_SizeDistributionFitType_Key, "Size Distribution Fit Type", "", 0, ChoicesParameter::Choices{"Option 1", "Option 2", "Option 3"}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_BiasedFeaturesArrayPath_Key, "Biased Features", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_EquivalentDiametersArrayPath_Key, "Equivalent Diameters", "", DataPath{}));
  params.insert(std::make_unique<ChoicesParameter>(k_AspectRatioDistributionFitType_Key, "Aspect Ratio Distribution Fit Type", "", 0, ChoicesParameter::Choices{"Option 1", "Option 2", "Option 3"}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_AspectRatiosArrayPath_Key, "Aspect Ratios", "", DataPath{}));
  params.insert(std::make_unique<ChoicesParameter>(k_Omega3DistributionFitType_Key, "Omega3 Distribution Fit Type", "", 0, ChoicesParameter::Choices{"Option 1", "Option 2", "Option 3"}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_Omega3sArrayPath_Key, "Omega3s", "", DataPath{}));
  params.insert(std::make_unique<ChoicesParameter>(k_NeighborhoodDistributionFitType_Key, "Neighborhood Distribution Fit Type", "", 0, ChoicesParameter::Choices{"Option 1", "Option 2", "Option 3"}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_NeighborhoodsArrayPath_Key, "Neighborhoods", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_AxisEulerAnglesArrayPath_Key, "Axis Euler Angles", "", DataPath{}));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_CalculateCrystallographicStats_Key, "Calculate Crystallographic Statistics", "", false));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SurfaceFeaturesArrayPath_Key, "Surface Features", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_VolumesArrayPath_Key, "Volumes", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureEulerAnglesArrayPath_Key, "Average Euler Angles", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_AvgQuatsArrayPath_Key, "Average Quaternions", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_SharedSurfaceAreaListArrayPath_Key, "Shared Surface Area List", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Ensemble Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_CrystalStructuresArrayPath_Key, "Crystal Structures", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Ensemble Data"});
  params.insert(std::make_unique<ArrayCreationParameter>(k_PhaseTypesArrayName_Key, "Phase Types", "", DataPath{}));
  params.insert(std::make_unique<ArrayCreationParameter>(k_StatisticsArrayName_Key, "Statistics", "", DataPath{}));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_IncludeRadialDistFunc_Key, "Include Radial Distribution Function", "", false));
  params.insert(std::make_unique<ArraySelectionParameter>(k_RDFArrayPath_Key, "Radial Distribution Function", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_MaxMinRDFArrayPath_Key, "Max and Min Separation Distances", "", DataPath{}));
  // Associate the Linkable Parameter(s) to the children parameters that they control
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_SizeDistributionFitType_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_BiasedFeaturesArrayPath_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_EquivalentDiametersArrayPath_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_AspectRatioDistributionFitType_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_AspectRatiosArrayPath_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_Omega3DistributionFitType_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_Omega3sArrayPath_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_NeighborhoodDistributionFitType_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_NeighborhoodsArrayPath_Key, true);
  params.linkParameters(k_CalculateMorphologicalStats_Key, k_AxisEulerAnglesArrayPath_Key, true);
  params.linkParameters(k_CalculateCrystallographicStats_Key, k_CrystalStructuresArrayPath_Key, true);
  params.linkParameters(k_CalculateCrystallographicStats_Key, k_SurfaceFeaturesArrayPath_Key, true);
  params.linkParameters(k_CalculateCrystallographicStats_Key, k_VolumesArrayPath_Key, true);
  params.linkParameters(k_CalculateCrystallographicStats_Key, k_FeatureEulerAnglesArrayPath_Key, true);
  params.linkParameters(k_CalculateCrystallographicStats_Key, k_AvgQuatsArrayPath_Key, true);
  params.linkParameters(k_CalculateCrystallographicStats_Key, k_SharedSurfaceAreaListArrayPath_Key, true);
  params.linkParameters(k_CalculateCrystallographicStats_Key, k_CrystalStructuresArrayPath_Key, true);
  params.linkParameters(k_IncludeRadialDistFunc_Key, k_RDFArrayPath_Key, true);
  params.linkParameters(k_IncludeRadialDistFunc_Key, k_MaxMinRDFArrayPath_Key, true);

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer GenerateEnsembleStatistics::clone() const
{
  return std::make_unique<GenerateEnsembleStatistics>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult GenerateEnsembleStatistics::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/

  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pPhaseTypeArrayValue = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k_PhaseTypeArray_Key);
  auto pSizeCorrelationResolutionValue = filterArgs.value<float32>(k_SizeCorrelationResolution_Key);
  auto pFeaturePhasesArrayPathValue = filterArgs.value<DataPath>(k_FeaturePhasesArrayPath_Key);
  auto pNeighborListArrayPathValue = filterArgs.value<DataPath>(k_NeighborListArrayPath_Key);
  auto pCalculateMorphologicalStatsValue = filterArgs.value<bool>(k_CalculateMorphologicalStats_Key);
  auto pSizeDistributionFitTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_SizeDistributionFitType_Key);
  auto pBiasedFeaturesArrayPathValue = filterArgs.value<DataPath>(k_BiasedFeaturesArrayPath_Key);
  auto pEquivalentDiametersArrayPathValue = filterArgs.value<DataPath>(k_EquivalentDiametersArrayPath_Key);
  auto pAspectRatioDistributionFitTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_AspectRatioDistributionFitType_Key);
  auto pAspectRatiosArrayPathValue = filterArgs.value<DataPath>(k_AspectRatiosArrayPath_Key);
  auto pOmega3DistributionFitTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_Omega3DistributionFitType_Key);
  auto pOmega3sArrayPathValue = filterArgs.value<DataPath>(k_Omega3sArrayPath_Key);
  auto pNeighborhoodDistributionFitTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_NeighborhoodDistributionFitType_Key);
  auto pNeighborhoodsArrayPathValue = filterArgs.value<DataPath>(k_NeighborhoodsArrayPath_Key);
  auto pAxisEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_AxisEulerAnglesArrayPath_Key);
  auto pCalculateCrystallographicStatsValue = filterArgs.value<bool>(k_CalculateCrystallographicStats_Key);
  auto pSurfaceFeaturesArrayPathValue = filterArgs.value<DataPath>(k_SurfaceFeaturesArrayPath_Key);
  auto pVolumesArrayPathValue = filterArgs.value<DataPath>(k_VolumesArrayPath_Key);
  auto pFeatureEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_FeatureEulerAnglesArrayPath_Key);
  auto pAvgQuatsArrayPathValue = filterArgs.value<DataPath>(k_AvgQuatsArrayPath_Key);
  auto pSharedSurfaceAreaListArrayPathValue = filterArgs.value<DataPath>(k_SharedSurfaceAreaListArrayPath_Key);
  auto pCrystalStructuresArrayPathValue = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  auto pPhaseTypesArrayNameValue = filterArgs.value<DataPath>(k_PhaseTypesArrayName_Key);
  auto pStatisticsArrayNameValue = filterArgs.value<DataPath>(k_StatisticsArrayName_Key);
  auto pIncludeRadialDistFuncValue = filterArgs.value<bool>(k_IncludeRadialDistFunc_Key);
  auto pRDFArrayPathValue = filterArgs.value<DataPath>(k_RDFArrayPath_Key);
  auto pMaxMinRDFArrayPathValue = filterArgs.value<DataPath>(k_MaxMinRDFArrayPath_Key);

  // Declare the preflightResult variable that will be populated with the results
  // of the preflight. The PreflightResult type contains the output Actions and
  // any preflight updated values that you want to be displayed to the user, typically
  // through a user interface (UI).
  PreflightResult preflightResult;

  // If your filter is making structural changes to the DataStructure then the filter
  // is going to create OutputActions subclasses that need to be returned. This will
  // store those actions.
  complex::Result<OutputActions> resultOutputActions;

  // If your filter is going to pass back some `preflight updated values` then this is where you
  // would create the code to store those values in the appropriate object. Note that we
  // in line creating the pair (NOT a std::pair<>) of Key:Value that will get stored in
  // the std::vector<PreflightValue> object.
  std::vector<PreflightValue> preflightUpdatedValues;

  // If the filter needs to pass back some updated values via a key:value string:string set of values
  // you can declare and update that string here.

  // Assuming this filter did make some structural changes to the DataStructure then store
  // the outputAction into the resultOutputActions object via a std::move().
  // NOTE: That using std::move() means that you can *NOT* use the outputAction variable
  // past this point so let us scope this part which will stop stupid subtle bugs
  // from being introduced. If you have multiple `Actions` classes that you are
  // using such as a CreateDataGroupAction followed by a CreateArrayAction you might
  // want to consider scoping each of those bits of code into their own section of code
  {
    // Replace the "EmptyAction" with one of the prebuilt actions that apply changes
    // to the DataStructure. If none are available then create a new custom Action subclass.
    // If your filter does not make any structural modifications to the DataStructure then
    // you can skip this code.

    auto outputAction = std::make_unique<EmptyAction>();
    resultOutputActions.value().actions.push_back(std::move(outputAction));
  }

  // Store the preflight updated value(s) into the preflightUpdatedValues vector using
  // the appropriate methods.

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> GenerateEnsembleStatistics::executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pPhaseTypeArrayValue = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k_PhaseTypeArray_Key);
  auto pSizeCorrelationResolutionValue = filterArgs.value<float32>(k_SizeCorrelationResolution_Key);
  auto pFeaturePhasesArrayPathValue = filterArgs.value<DataPath>(k_FeaturePhasesArrayPath_Key);
  auto pNeighborListArrayPathValue = filterArgs.value<DataPath>(k_NeighborListArrayPath_Key);
  auto pCalculateMorphologicalStatsValue = filterArgs.value<bool>(k_CalculateMorphologicalStats_Key);
  auto pSizeDistributionFitTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_SizeDistributionFitType_Key);
  auto pBiasedFeaturesArrayPathValue = filterArgs.value<DataPath>(k_BiasedFeaturesArrayPath_Key);
  auto pEquivalentDiametersArrayPathValue = filterArgs.value<DataPath>(k_EquivalentDiametersArrayPath_Key);
  auto pAspectRatioDistributionFitTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_AspectRatioDistributionFitType_Key);
  auto pAspectRatiosArrayPathValue = filterArgs.value<DataPath>(k_AspectRatiosArrayPath_Key);
  auto pOmega3DistributionFitTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_Omega3DistributionFitType_Key);
  auto pOmega3sArrayPathValue = filterArgs.value<DataPath>(k_Omega3sArrayPath_Key);
  auto pNeighborhoodDistributionFitTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_NeighborhoodDistributionFitType_Key);
  auto pNeighborhoodsArrayPathValue = filterArgs.value<DataPath>(k_NeighborhoodsArrayPath_Key);
  auto pAxisEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_AxisEulerAnglesArrayPath_Key);
  auto pCalculateCrystallographicStatsValue = filterArgs.value<bool>(k_CalculateCrystallographicStats_Key);
  auto pSurfaceFeaturesArrayPathValue = filterArgs.value<DataPath>(k_SurfaceFeaturesArrayPath_Key);
  auto pVolumesArrayPathValue = filterArgs.value<DataPath>(k_VolumesArrayPath_Key);
  auto pFeatureEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_FeatureEulerAnglesArrayPath_Key);
  auto pAvgQuatsArrayPathValue = filterArgs.value<DataPath>(k_AvgQuatsArrayPath_Key);
  auto pSharedSurfaceAreaListArrayPathValue = filterArgs.value<DataPath>(k_SharedSurfaceAreaListArrayPath_Key);
  auto pCrystalStructuresArrayPathValue = filterArgs.value<DataPath>(k_CrystalStructuresArrayPath_Key);
  auto pPhaseTypesArrayNameValue = filterArgs.value<DataPath>(k_PhaseTypesArrayName_Key);
  auto pStatisticsArrayNameValue = filterArgs.value<DataPath>(k_StatisticsArrayName_Key);
  auto pIncludeRadialDistFuncValue = filterArgs.value<bool>(k_IncludeRadialDistFunc_Key);
  auto pRDFArrayPathValue = filterArgs.value<DataPath>(k_RDFArrayPath_Key);
  auto pMaxMinRDFArrayPathValue = filterArgs.value<DataPath>(k_MaxMinRDFArrayPath_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
