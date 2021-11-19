#include "ApplyTransformationToGeometry.hpp"

#include "complex/DataStructure/DataPath.hpp"
#include "complex/Filter/Actions/EmptyAction.hpp"
#include "complex/Parameters/ArraySelectionParameter.hpp"
#include "complex/Parameters/ChoicesParameter.hpp"
#include "complex/Parameters/DataGroupSelectionParameter.hpp"
#include "complex/Parameters/DynamicTableFilterParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/VectorParameter.hpp"

using namespace complex;

namespace complex
{
//------------------------------------------------------------------------------
std::string ApplyTransformationToGeometry::name() const
{
  return FilterTraits<ApplyTransformationToGeometry>::name.str();
}

//------------------------------------------------------------------------------
std::string ApplyTransformationToGeometry::className() const
{
  return FilterTraits<ApplyTransformationToGeometry>::className;
}

//------------------------------------------------------------------------------
Uuid ApplyTransformationToGeometry::uuid() const
{
  return FilterTraits<ApplyTransformationToGeometry>::uuid;
}

//------------------------------------------------------------------------------
std::string ApplyTransformationToGeometry::humanName() const
{
  return "Apply Transformation to Geometry";
}

//------------------------------------------------------------------------------
std::vector<std::string> ApplyTransformationToGeometry::defaultTags() const
{
  return {"#DREAM3D Review", "#Rotation/Transforming"};
}

//------------------------------------------------------------------------------
Parameters ApplyTransformationToGeometry::parameters() const
{
  Parameters params;
  // Create the parameter descriptors that are needed for this filter
  params.insertLinkableParameter(
      std::make_unique<ChoicesParameter>(k_TransformationMatrixType_Key, "Transformation Type", "", 0,
                                         ChoicesParameter::Choices{"No Transformation", "Pre-Computed Transformation Matrix", "Manual Transformation Matrix", "Rotation", "Translation", "Scale"}));
  /*[x]*/ params.insert(std::make_unique<DynamicTableFilterParameter>(k_ManualTransformationMatrix_Key, "Transformation Matrix", "", {}));
  params.insert(std::make_unique<Float32Parameter>(k_RotationAngle_Key, "Rotation Angle (Degrees)", "", 1.23345f));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_RotationAxis_Key, "Rotation Axis (ijk)", "", std::vector<float32>(3), std::vector<std::string>(3)));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_Translation_Key, "Translation", "", std::vector<float32>(3), std::vector<std::string>(3)));
  params.insert(std::make_unique<VectorFloat32Parameter>(k_Scale_Key, "Scale", "", std::vector<float32>(3), std::vector<std::string>(3)));
  params.insert(std::make_unique<DataGroupSelectionParameter>(k_GeometryToTransform_Key, "Geometry to Transform", "", DataPath{}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_ComputedTransformationMatrix_Key, "Transformation Matrix", "", DataPath{}));
  // Associate the Linkable Parameter(s) to the children parameters that they control
  params.linkParameters(k_TransformationMatrixType_Key, k_ComputedTransformationMatrix_Key, 1);
  params.linkParameters(k_TransformationMatrixType_Key, k_ManualTransformationMatrix_Key, 2);
  params.linkParameters(k_TransformationMatrixType_Key, k_RotationAngle_Key, 3);
  params.linkParameters(k_TransformationMatrixType_Key, k_RotationAxis_Key, 3);
  params.linkParameters(k_TransformationMatrixType_Key, k_Translation_Key, 4);
  params.linkParameters(k_TransformationMatrixType_Key, k_Scale_Key, 5);

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ApplyTransformationToGeometry::clone() const
{
  return std::make_unique<ApplyTransformationToGeometry>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ApplyTransformationToGeometry::preflightImpl(const DataStructure& ds, const Arguments& filterArgs, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Write any preflight sanity checking codes in this function
   ***************************************************************************/

  /**
   * These are the values that were gathered from the UI or the pipeline file or
   * otherwise passed into the filter. These are here for your convenience. If you
   * do not need some of them remove them.
   */
  auto pTransformationMatrixTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_TransformationMatrixType_Key);
  auto pManualTransformationMatrixValue = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k_ManualTransformationMatrix_Key);
  auto pRotationAngleValue = filterArgs.value<float32>(k_RotationAngle_Key);
  auto pRotationAxisValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_RotationAxis_Key);
  auto pTranslationValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Translation_Key);
  auto pScaleValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Scale_Key);
  auto pGeometryToTransformValue = filterArgs.value<DataPath>(k_GeometryToTransform_Key);
  auto pComputedTransformationMatrixValue = filterArgs.value<DataPath>(k_ComputedTransformationMatrix_Key);

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
Result<> ApplyTransformationToGeometry::executeImpl(DataStructure& data, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler) const
{
  /****************************************************************************
   * Extract the actual input values from the 'filterArgs' object
   ***************************************************************************/
  auto pTransformationMatrixTypeValue = filterArgs.value<ChoicesParameter::ValueType>(k_TransformationMatrixType_Key);
  auto pManualTransformationMatrixValue = filterArgs.value<<<<NOT_IMPLEMENTED>>>>(k_ManualTransformationMatrix_Key);
  auto pRotationAngleValue = filterArgs.value<float32>(k_RotationAngle_Key);
  auto pRotationAxisValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_RotationAxis_Key);
  auto pTranslationValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Translation_Key);
  auto pScaleValue = filterArgs.value<VectorFloat32Parameter::ValueType>(k_Scale_Key);
  auto pGeometryToTransformValue = filterArgs.value<DataPath>(k_GeometryToTransform_Key);
  auto pComputedTransformationMatrixValue = filterArgs.value<DataPath>(k_ComputedTransformationMatrix_Key);

  /****************************************************************************
   * Write your algorithm implementation in this function
   ***************************************************************************/

  return {};
}
} // namespace complex
