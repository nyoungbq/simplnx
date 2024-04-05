#include "WriteLosAlamosFFTFilter.hpp"

#include "SimplnxCore/Filters/Algorithms/WriteLosAlamosFFT.hpp"

#include "simplnx/Common/AtomicFile.hpp"
#include "simplnx/DataStructure/DataPath.hpp"
#include "simplnx/Filter/Actions/EmptyAction.hpp"
#include "simplnx/Parameters/ArraySelectionParameter.hpp"
#include "simplnx/Parameters/FileSystemPathParameter.hpp"
#include "simplnx/Parameters/GeometrySelectionParameter.hpp"

#include "simplnx/Utilities/SIMPLConversion.hpp"

#include <filesystem>

namespace fs = std::filesystem;
using namespace nx::core;

namespace nx::core
{
//------------------------------------------------------------------------------
std::string WriteLosAlamosFFTFilter::name() const
{
  return FilterTraits<WriteLosAlamosFFTFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string WriteLosAlamosFFTFilter::className() const
{
  return FilterTraits<WriteLosAlamosFFTFilter>::className;
}

//------------------------------------------------------------------------------
Uuid WriteLosAlamosFFTFilter::uuid() const
{
  return FilterTraits<WriteLosAlamosFFTFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string WriteLosAlamosFFTFilter::humanName() const
{
  return "Write Los Alamos FFT File";
}

//------------------------------------------------------------------------------
std::vector<std::string> WriteLosAlamosFFTFilter::defaultTags() const
{
  return {className(), "IO", "Output", "Write", "Export"};
}

//------------------------------------------------------------------------------
Parameters WriteLosAlamosFFTFilter::parameters() const
{
  Parameters params;

  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Input Parameters"});
  params.insert(std::make_unique<FileSystemPathParameter>(k_OutputFile_Key, "Output File Path", "The path to the output file", fs::path(""), FileSystemPathParameter::ExtensionsType{},
                                                          FileSystemPathParameter::PathType::OutputFile));

  params.insertSeparator(Parameters::Separator{"Required Data Objects"});
  params.insert(std::make_unique<GeometrySelectionParameter>(k_ImageGeomPath, "Parent Image Geometry", "The parent image geometry holding the subsequent arrays", DataPath{},
                                                             GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Image}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureIdsArrayPath_Key, "Feature Ids", "Data Array that specifies to which Feature each Element belongs", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::int32}, ArraySelectionParameter::AllowedComponentShapes{{1}}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CellEulerAnglesArrayPath_Key, "Euler Angles",
                                                          "Data Array containing the three angles defining the orientation for each of the Cell in Bunge convention (Z-X-Z)", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::float32}, ArraySelectionParameter::AllowedComponentShapes{{3}}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CellPhasesArrayPath_Key, "Feature Phases", "Data Array that specifies to which Ensemble each Cell belongs", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::int32}, ArraySelectionParameter::AllowedComponentShapes{{1}}));

  return params;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer WriteLosAlamosFFTFilter::clone() const
{
  return std::make_unique<WriteLosAlamosFFTFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult WriteLosAlamosFFTFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                const std::atomic_bool& shouldCancel) const
{
  auto pOutputFileValue = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputFile_Key);
  auto pFeatureIdsArrayPathValue = filterArgs.value<DataPath>(k_FeatureIdsArrayPath_Key);
  auto pCellEulerAnglesArrayPathValue = filterArgs.value<DataPath>(k_CellEulerAnglesArrayPath_Key);
  auto pCellPhasesArrayPathValue = filterArgs.value<DataPath>(k_CellPhasesArrayPath_Key);

  PreflightResult preflightResult;
  nx::core::Result<OutputActions> resultOutputActions;
  std::vector<PreflightValue> preflightUpdatedValues;

  usize fIdsTupCount = dataStructure.getDataAs<IDataArray>(pFeatureIdsArrayPathValue)->getNumberOfTuples();
  usize eulersTupCount = dataStructure.getDataAs<IDataArray>(pCellEulerAnglesArrayPathValue)->getNumberOfTuples();
  usize phasesTupCount = dataStructure.getDataAs<IDataArray>(pCellPhasesArrayPathValue)->getNumberOfTuples();

  if(fIdsTupCount != eulersTupCount || fIdsTupCount != phasesTupCount)
  {
    return MakePreflightErrorResult(-73460, fmt::format("Tuple Dimensions don't match: Feature Ids - {} || Euler Angles - {} || Phases - {}", fIdsTupCount, eulersTupCount, phasesTupCount));
  }

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> WriteLosAlamosFFTFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                              const std::atomic_bool& shouldCancel) const
{
  AtomicFile atomicFile(filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputFile_Key));
  auto creationResult = atomicFile.getResult();
  if(creationResult.invalid())
  {
    return creationResult;
  }

  WriteLosAlamosFFTInputValues inputValues;

  inputValues.OutputFile = atomicFile.tempFilePath();
  inputValues.FeatureIdsArrayPath = filterArgs.value<DataPath>(k_FeatureIdsArrayPath_Key);
  inputValues.CellEulerAnglesArrayPath = filterArgs.value<DataPath>(k_CellEulerAnglesArrayPath_Key);
  inputValues.CellPhasesArrayPath = filterArgs.value<DataPath>(k_CellPhasesArrayPath_Key);
  inputValues.ImageGeomPath = filterArgs.value<DataPath>(k_ImageGeomPath);

  auto result = WriteLosAlamosFFT(dataStructure, messageHandler, shouldCancel, &inputValues)();
  if(result.valid())
  {
    if(!atomicFile.commit())
    {
      return atomicFile.getResult();
    }
  }
  return result;
}

namespace
{
namespace SIMPL
{
constexpr StringLiteral k_OutputFileKey = "OutputFile";
constexpr StringLiteral k_FeatureIdsArrayPathKey = "FeatureIdsArrayPath";
constexpr StringLiteral k_CellEulerAnglesArrayPathKey = "CellEulerAnglesArrayPath";
constexpr StringLiteral k_CellPhasesArrayPathKey = "CellPhasesArrayPath";
} // namespace SIMPL
} // namespace

Result<Arguments> WriteLosAlamosFFTFilter::FromSIMPLJson(const nlohmann::json& json)
{
  Arguments args = WriteLosAlamosFFTFilter().getDefaultArguments();

  std::vector<Result<>> results;

  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::OutputFileFilterParameterConverter>(args, json, SIMPL::k_OutputFileKey, k_OutputFile_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataContainerSelectionFilterParameterConverter>(args, json, SIMPL::k_FeatureIdsArrayPathKey, k_ImageGeomPath));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_FeatureIdsArrayPathKey, k_FeatureIdsArrayPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_CellEulerAnglesArrayPathKey, k_CellEulerAnglesArrayPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_CellPhasesArrayPathKey, k_CellPhasesArrayPath_Key));

  Result<> conversionResult = MergeResults(std::move(results));

  return ConvertResultTo<Arguments>(std::move(conversionResult), std::move(args));
}
} // namespace nx::core
