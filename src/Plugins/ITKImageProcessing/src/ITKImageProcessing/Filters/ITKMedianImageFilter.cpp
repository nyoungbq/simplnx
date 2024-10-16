#include "ITKMedianImageFilter.hpp"

#include "ITKImageProcessing/Common/ITKArrayHelper.hpp"
#include "ITKImageProcessing/Common/sitkCommon.hpp"

#include "simplnx/Parameters/ArraySelectionParameter.hpp"
#include "simplnx/Parameters/DataGroupSelectionParameter.hpp"
#include "simplnx/Parameters/DataObjectNameParameter.hpp"
#include "simplnx/Parameters/GeometrySelectionParameter.hpp"
#include "simplnx/Parameters/StringParameter.hpp"
#include "simplnx/Parameters/VectorParameter.hpp"

#include "simplnx/Utilities/SIMPLConversion.hpp"

#include <itkMedianImageFilter.h>

using namespace nx::core;

namespace cxITKMedianImageFilter
{
using ArrayOptionsType = ITK::ScalarPixelIdTypeList;
// VectorPixelIDTypeList;

struct ITKMedianImageFunctor
{
  using RadiusInputRadiusType = std::vector<uint32>;
  RadiusInputRadiusType radius = std::vector<unsigned int>(3, 1);

  template <class InputImageT, class OutputImageT, uint32 Dimension>
  auto createFilter() const
  {
    using FilterType = itk::MedianImageFilter<InputImageT, OutputImageT>;
    auto filter = FilterType::New();
    // Set the Radius Filter Property
    {
      using RadiusType = typename FilterType::RadiusType;
      auto convertedRadius = ITK::CastVec3ToITK<RadiusType, typename RadiusType::SizeValueType>(radius, RadiusType::Dimension);
      filter->SetRadius(convertedRadius);
    }

    return filter;
  }
};
} // namespace cxITKMedianImageFilter

namespace nx::core
{
//------------------------------------------------------------------------------
std::string ITKMedianImageFilter::name() const
{
  return FilterTraits<ITKMedianImageFilter>::name;
}

//------------------------------------------------------------------------------
std::string ITKMedianImageFilter::className() const
{
  return FilterTraits<ITKMedianImageFilter>::className;
}

//------------------------------------------------------------------------------
Uuid ITKMedianImageFilter::uuid() const
{
  return FilterTraits<ITKMedianImageFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string ITKMedianImageFilter::humanName() const
{
  return "ITK Median Image Filter";
}

//------------------------------------------------------------------------------
std::vector<std::string> ITKMedianImageFilter::defaultTags() const
{
  return {className(), "ITKImageProcessing", "ITKMedianImage", "ITKSmoothing", "Smoothing"};
}

//------------------------------------------------------------------------------
Parameters ITKMedianImageFilter::parameters() const
{
  Parameters params;
  params.insertSeparator(Parameters::Separator{"Input Parameter(s)"});
  params.insert(std::make_unique<VectorUInt32Parameter>(k_Radius_Key, "Radius", "Radius Dimensions XYZ", std::vector<unsigned int>(3, 1), std::vector<std::string>{"X", "Y", "Z"}));

  params.insertSeparator(Parameters::Separator{"Input Cell Data"});
  params.insert(std::make_unique<GeometrySelectionParameter>(k_InputImageGeomPath_Key, "Image Geometry", "Select the Image Geometry Group from the DataStructure.", DataPath({"Image Geometry"}),
                                                             GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Image}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_InputImageDataPath_Key, "Input Cell Data", "The image data that will be processed by this filter.", DataPath{},
                                                          nx::core::ITK::GetScalarPixelAllowedTypes()));

  params.insertSeparator(Parameters::Separator{"Output Cell Data"});
  params.insert(std::make_unique<DataObjectNameParameter>(k_OutputImageArrayName_Key, "Output Cell Data",
                                                          "The result of the processing will be stored in this Data Array inside the same group as the input data.", "Output Image Data"));

  return params;
}

//------------------------------------------------------------------------------
IFilter::VersionType ITKMedianImageFilter::parametersVersion() const
{
  return 1;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ITKMedianImageFilter::clone() const
{
  return std::make_unique<ITKMedianImageFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ITKMedianImageFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                             const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_InputImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_InputImageDataPath_Key);
  auto outputArrayName = filterArgs.value<DataObjectNameParameter::ValueType>(k_OutputImageArrayName_Key);
  auto radius = filterArgs.value<VectorUInt32Parameter::ValueType>(k_Radius_Key);

  const DataPath outputArrayPath = selectedInputArray.replaceName(outputArrayName);

  Result<OutputActions> resultOutputActions = ITK::DataCheck<cxITKMedianImageFilter::ArrayOptionsType>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath);

  return {std::move(resultOutputActions)};
}

//------------------------------------------------------------------------------
Result<> ITKMedianImageFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                           const std::atomic_bool& shouldCancel) const
{
  auto imageGeomPath = filterArgs.value<DataPath>(k_InputImageGeomPath_Key);
  auto selectedInputArray = filterArgs.value<DataPath>(k_InputImageDataPath_Key);
  auto outputArrayName = filterArgs.value<DataObjectNameParameter::ValueType>(k_OutputImageArrayName_Key);
  const DataPath outputArrayPath = selectedInputArray.replaceName(outputArrayName);

  auto radius = filterArgs.value<VectorUInt32Parameter::ValueType>(k_Radius_Key);

  const cxITKMedianImageFilter::ITKMedianImageFunctor itkFunctor = {radius};

  auto& imageGeom = dataStructure.getDataRefAs<ImageGeom>(imageGeomPath);

  return ITK::Execute<cxITKMedianImageFilter::ArrayOptionsType>(dataStructure, selectedInputArray, imageGeomPath, outputArrayPath, itkFunctor, shouldCancel);
}

namespace
{
namespace SIMPL
{
constexpr StringLiteral k_RadiusKey = "Radius";
constexpr StringLiteral k_SelectedCellArrayPathKey = "SelectedCellArrayPath";
constexpr StringLiteral k_NewCellArrayNameKey = "NewCellArrayName";
} // namespace SIMPL
} // namespace

Result<Arguments> ITKMedianImageFilter::FromSIMPLJson(const nlohmann::json& json)
{
  Arguments args = ITKMedianImageFilter().getDefaultArguments();

  std::vector<Result<>> results;

  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::UInt32Vec3FilterParameterConverter>(args, json, SIMPL::k_RadiusKey, k_Radius_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataContainerSelectionFilterParameterConverter>(args, json, SIMPL::k_SelectedCellArrayPathKey, k_InputImageGeomPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_SelectedCellArrayPathKey, k_InputImageDataPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::StringFilterParameterConverter>(args, json, SIMPL::k_NewCellArrayNameKey, k_OutputImageArrayName_Key));

  Result<> conversionResult = MergeResults(std::move(results));

  return ConvertResultTo<Arguments>(std::move(conversionResult), std::move(args));
}
} // namespace nx::core
