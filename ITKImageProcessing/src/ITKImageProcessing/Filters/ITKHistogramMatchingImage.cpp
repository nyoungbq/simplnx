#include "ITKHistogramMatchingImage.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/EmptyAction.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string ITKHistogramMatchingImage::name() const
{
  return FilterTraits<ITKHistogramMatchingImage>::name.str();
}

//------------------------------------------------------------------------------
std::string ITKHistogramMatchingImage::className() const
{
  return FilterTraits<ITKHistogramMatchingImage>::className;
}

//------------------------------------------------------------------------------
Uuid ITKHistogramMatchingImage::uuid() const
{
  return FilterTraits<ITKHistogramMatchingImage>::uuid;
}

//------------------------------------------------------------------------------
std::string ITKHistogramMatchingImage::humanName() const
{
  return "ITK::Histogram Matching Image Filter";
}

//------------------------------------------------------------------------------
std::vector<std::string> ITKHistogramMatchingImage::defaultTags() const
{
  return {"#ITK Image Processing", "#ITK IntensityTransformation"};
}

//------------------------------------------------------------------------------
Parameters ITKHistogramMatchingImage::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insert(std::make_unique<Float64Parameter>(k_NumberOfHistogramLevels_Key, "NumberOfHistogramLevels", "", 2.3456789));
  params.insert(std::make_unique<Float64Parameter>(k_NumberOfMatchPoints_Key, "NumberOfMatchPoints", "", 2.3456789));
  params.insert(std::make_unique<BoolParameter>(k_ThresholdAtMeanIntensity_Key, "ThresholdAtMeanIntensity", "", false));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(k_SelectedCellArrayPath_Key, "Input Attribute Array to filter", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_ReferenceCellArrayPath_Key, "Reference Attribute Array to filter", "", DataPath{}));
  params.insertSeparator(Parameters::Separator{"Cell Data"});
  params.insert(std::make_unique<StringParameter>(k_NewCellArrayName_Key, "Filtered Array", "", "SomeString"));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ITKHistogramMatchingImage::clone() const
{
  return std::make_unique<ITKHistogramMatchingImage>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ITKHistogramMatchingImage::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/

  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pNumberOfHistogramLevelsValue = filterArgs.value<float64>(k_NumberOfHistogramLevels_Key);
  auto pNumberOfMatchPointsValue = filterArgs.value<float64>(k_NumberOfMatchPoints_Key);
  auto pThresholdAtMeanIntensityValue = filterArgs.value<bool>(k_ThresholdAtMeanIntensity_Key);
  auto pSelectedCellArrayPathValue = filterArgs.value<DataPath>(k_SelectedCellArrayPath_Key);
  auto pReferenceCellArrayPathValue = filterArgs.value<DataPath>(k_ReferenceCellArrayPath_Key);
  auto pNewCellArrayNameValue = filterArgs.value<StringParameter::ValueType>(k_NewCellArrayName_Key);

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
Result<> ITKHistogramMatchingImage::executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pNumberOfHistogramLevelsValue = filterArgs.value<float64>(k_NumberOfHistogramLevels_Key);
  auto pNumberOfMatchPointsValue = filterArgs.value<float64>(k_NumberOfMatchPoints_Key);
  auto pThresholdAtMeanIntensityValue = filterArgs.value<bool>(k_ThresholdAtMeanIntensity_Key);
  auto pSelectedCellArrayPathValue = filterArgs.value<DataPath>(k_SelectedCellArrayPath_Key);
  auto pReferenceCellArrayPathValue = filterArgs.value<DataPath>(k_ReferenceCellArrayPath_Key);
  auto pNewCellArrayNameValue = filterArgs.value<StringParameter::ValueType>(k_NewCellArrayName_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
