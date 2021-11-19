#include "ConvertColorToGrayScale.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/EmptyAction.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/MultiArraySelectionParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string ConvertColorToGrayScale::name() const
{
  return FilterTraits<ConvertColorToGrayScale>::name.str();
}

//------------------------------------------------------------------------------
std::string ConvertColorToGrayScale::className() const
{
  return FilterTraits<ConvertColorToGrayScale>::className;
}

//------------------------------------------------------------------------------
Uuid ConvertColorToGrayScale::uuid() const
{
  return FilterTraits<ConvertColorToGrayScale>::uuid;
}

//------------------------------------------------------------------------------
std::string ConvertColorToGrayScale::humanName() const
{
  return "Color to GrayScale";
}

//------------------------------------------------------------------------------
std::vector<std::string> ConvertColorToGrayScale::defaultTags() const
{
  return {"#Core", "#Image"};
}

//------------------------------------------------------------------------------
Parameters ConvertColorToGrayScale::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insertLinkableParameter(
      std::make_unique<ChoicesParameter>(k_ConversionAlgorithm_Key, "Conversion Algorithm", "", 0, ChoicesParameter::Choices{"Luminosity", "Average", "Lightness", "SingleChannel"}));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_ColorWeights_Key, "Color Weighting", "", std::vector<float32>(3), std::vector<std::string>(3)));
  params.insert(std::make_unique<Int32Parameter>(k_ColorChannel_Key, "Color Channel", "", 1234356));
  params.insert(std::make_unique<MultiArraySelectionParameter>(k_InputDataArrayVector_Key, "Input Attribute Arrays", "", MultiArraySelectionParameter::ValueType{DataPath(), DataPath(), DataPath()}));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_CreateNewAttributeMatrix_Key, "Create Attribute Matrix", "", false));
  params.insert(std::make_unique<StringParameter>(k_OutputAttributeMatrixName_Key, "Output Cell Attribute Matrix", "", "SomeString"));
  params.insert(std::make_unique<StringParameter>(k_OutputArrayPrefix_Key, "Output Array Prefix", "", "SomeString"));
  // Associate the Linkable Parameter(s) to the children parameters that they control
  params.linkParameters(k_ConversionAlgorithm_Key, k_ColorWeights_Key, 0);
  params.linkParameters(k_ConversionAlgorithm_Key, k_ColorChannel_Key, 3);
  params.linkParameters(k_CreateNewAttributeMatrix_Key, k_OutputAttributeMatrixName_Key, true);

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ConvertColorToGrayScale::clone() const
{
  return std::make_unique<ConvertColorToGrayScale>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ConvertColorToGrayScale::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/

  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pConversionAlgorithmValue = filterArgs.value<ChoicesParameter::ValueType>(k_ConversionAlgorithm_Key);
  auto pColorWeightsValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_ColorWeights_Key);
  auto pColorChannelValue = filterArgs.value<int32>(k_ColorChannel_Key);
  auto pInputDataArrayVectorValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_InputDataArrayVector_Key);
  auto pCreateNewAttributeMatrixValue = filterArgs.value<bool>(k_CreateNewAttributeMatrix_Key);
  auto pOutputAttributeMatrixNameValue = filterArgs.value<StringParameter::ValueType>(k_OutputAttributeMatrixName_Key);
  auto pOutputArrayPrefixValue = filterArgs.value<StringParameter::ValueType>(k_OutputArrayPrefix_Key);

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
Result<> ConvertColorToGrayScale::executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pConversionAlgorithmValue = filterArgs.value<ChoicesParameter::ValueType>(k_ConversionAlgorithm_Key);
  auto pColorWeightsValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_ColorWeights_Key);
  auto pColorChannelValue = filterArgs.value<int32>(k_ColorChannel_Key);
  auto pInputDataArrayVectorValue = filterArgs.value<MultiArraySelectionParameter::ValueType>(k_InputDataArrayVector_Key);
  auto pCreateNewAttributeMatrixValue = filterArgs.value<bool>(k_CreateNewAttributeMatrix_Key);
  auto pOutputAttributeMatrixNameValue = filterArgs.value<StringParameter::ValueType>(k_OutputAttributeMatrixName_Key);
  auto pOutputArrayPrefixValue = filterArgs.value<StringParameter::ValueType>(k_OutputArrayPrefix_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
