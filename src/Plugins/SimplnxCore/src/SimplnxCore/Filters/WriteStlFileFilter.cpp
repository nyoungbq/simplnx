#include "WriteStlFileFilter.hpp"

#include "SimplnxCore/Filters/Algorithms/WriteStlFile.hpp"

#include "simplnx/DataStructure/DataPath.hpp"
#include "simplnx/DataStructure/Geometry/TriangleGeom.hpp"
#include "simplnx/Filter/Actions/EmptyAction.hpp"
#include "simplnx/Parameters/ArraySelectionParameter.hpp"
#include "simplnx/Parameters/ChoicesParameter.hpp"
#include "simplnx/Parameters/DataGroupSelectionParameter.hpp"
#include "simplnx/Parameters/FileSystemPathParameter.hpp"
#include "simplnx/Parameters/GeometrySelectionParameter.hpp"
#include "simplnx/Parameters/StringParameter.hpp"
#include "simplnx/Utilities/SIMPLConversion.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace nx::core;

namespace nx::core
{
//------------------------------------------------------------------------------
std::string WriteStlFileFilter::name() const
{
  return FilterTraits<WriteStlFileFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string WriteStlFileFilter::className() const
{
  return FilterTraits<WriteStlFileFilter>::className;
}

//------------------------------------------------------------------------------
Uuid WriteStlFileFilter::uuid() const
{
  return FilterTraits<WriteStlFileFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string WriteStlFileFilter::humanName() const
{
  return "Write STL Files from Triangle Geometry";
}

//------------------------------------------------------------------------------
std::vector<std::string> WriteStlFileFilter::defaultTags() const
{
  return {className(), "IO", "Output", "Write", "Export", "Triangles", "SurfaceMesh"};
}

//------------------------------------------------------------------------------
Parameters WriteStlFileFilter::parameters() const
{
  Parameters params;

  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Input Parameter(s)"});
  params.insertLinkableParameter(std::make_unique<ChoicesParameter>(k_GroupingType_Key, "File Grouping Type", "How to partition the stl files", to_underlying(GroupingType::Features),
                                                                    ChoicesParameter::Choices{"Features", "Phases and Features", "Single File", "Part Index"})); // sequence dependent DO NOT REORDER
  params.insert(std::make_unique<FileSystemPathParameter>(k_OutputStlDirectory_Key, "Output STL Directory", "Directory to dump the STL file(s) to", fs::path(),
                                                          FileSystemPathParameter::ExtensionsType{}, FileSystemPathParameter::PathType::OutputDir, true));
  params.insert(std::make_unique<StringParameter>(k_OutputStlPrefix_Key, "Output STL File Prefix",
                                                  "The prefix name of created files (other values will be appended later - including the .stl extension)", "Triangle"));

  params.insert(std::make_unique<FileSystemPathParameter>(k_OutputStlFile_Key, "Output STL File", "STL File to dump the Triangle Geometry to", fs::path(),
                                                          FileSystemPathParameter::ExtensionsType{".stl"}, FileSystemPathParameter::PathType::OutputFile, false));

  params.insertSeparator(Parameters::Separator{"Input Data Objects"});
  params.insert(std::make_unique<GeometrySelectionParameter>(k_TriangleGeomPath_Key, "Selected Triangle Geometry", "The geometry to print", DataPath{},
                                                             GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Triangle}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeatureIdsPath_Key, "Face labels", "The triangle feature ids array to order/index files by", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::int32}, ArraySelectionParameter::AllowedComponentShapes{{2}}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeaturePhasesPath_Key, "Feature Phases", "The feature phases array to further order/index files by", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::int32}, ArraySelectionParameter::AllowedComponentShapes{{1}}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_PartNumberPath_Key, "Part Numbers", "The Part Numbers to order/index files by", DataPath{},
                                                          ArraySelectionParameter::AllowedTypes{DataType::int32}, ArraySelectionParameter::AllowedComponentShapes{{1}}));
  // link params -- GroupingType enum is stored in the algorithm header [WriteStlFile.hpp]
  //------------ Group by Features -------------
  params.linkParameters(k_GroupingType_Key, k_OutputStlDirectory_Key, to_underlying(GroupingType::Features));
  params.linkParameters(k_GroupingType_Key, k_OutputStlPrefix_Key, to_underlying(GroupingType::Features));
  params.linkParameters(k_GroupingType_Key, k_FeatureIdsPath_Key, to_underlying(GroupingType::Features));

  //------- Group by Features and Phases -------
  params.linkParameters(k_GroupingType_Key, k_OutputStlDirectory_Key, to_underlying(GroupingType::FeaturesAndPhases));
  params.linkParameters(k_GroupingType_Key, k_OutputStlPrefix_Key, to_underlying(GroupingType::FeaturesAndPhases));
  params.linkParameters(k_GroupingType_Key, k_FeatureIdsPath_Key, to_underlying(GroupingType::FeaturesAndPhases));
  params.linkParameters(k_GroupingType_Key, k_FeaturePhasesPath_Key, to_underlying(GroupingType::FeaturesAndPhases));

  //--------------- Single File ----------------
  params.linkParameters(k_GroupingType_Key, k_OutputStlFile_Key, to_underlying(GroupingType::SingleFile));

  //--------------- Part Number ----------------
  params.linkParameters(k_GroupingType_Key, k_OutputStlDirectory_Key, to_underlying(GroupingType::PartNumber));
  params.linkParameters(k_GroupingType_Key, k_OutputStlPrefix_Key, to_underlying(GroupingType::PartNumber));
  params.linkParameters(k_GroupingType_Key, k_PartNumberPath_Key, to_underlying(GroupingType::PartNumber));

  return params;
}

//------------------------------------------------------------------------------
IFilter::VersionType WriteStlFileFilter::parametersVersion() const
{
  return 1;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer WriteStlFileFilter::clone() const
{
  return std::make_unique<WriteStlFileFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult WriteStlFileFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                           const std::atomic_bool& shouldCancel) const
{
  auto pGroupingTypeValue = static_cast<GroupingType>(filterArgs.value<ChoicesParameter::ValueType>(k_GroupingType_Key));
  auto pOutputStlDirectoryValue = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputStlDirectory_Key);
  auto pTriangleGeomPathValue = filterArgs.value<DataPath>(k_TriangleGeomPath_Key);
  auto pFeatureIdsPathValue = filterArgs.value<DataPath>(k_FeatureIdsPath_Key);
  auto pFeaturePhasesPathValue = filterArgs.value<DataPath>(k_FeaturePhasesPath_Key);
  auto pPartNumberPathValue = filterArgs.value<DataPath>(k_PartNumberPath_Key);

  nx::core::Result<OutputActions> resultOutputActions;

  auto* triangleGeom = dataStructure.getDataAs<TriangleGeom>(pTriangleGeomPathValue);
  if(triangleGeom == nullptr)
  {
    return MakePreflightErrorResult(-27870, fmt::format("Triangle Geometry doesn't exist at: {}", pTriangleGeomPathValue.toString()));
  }

  if(triangleGeom->getNumberOfFaces() > std::numeric_limits<int32>::max())
  {
    return MakePreflightErrorResult(
        -27871, fmt::format("The number of triangles is {}, but the STL specification only supports triangle counts up to {}", triangleGeom->getNumberOfFaces(), std::numeric_limits<int32>::max()));
  }

  if(pGroupingTypeValue == GroupingType::Features || pGroupingTypeValue == GroupingType::FeaturesAndPhases)
  {
    if(auto* featureIds = dataStructure.getDataAs<Int32Array>(pFeatureIdsPathValue); featureIds == nullptr)
    {
      return MakePreflightErrorResult(-27873, fmt::format("Feature Ids Array doesn't exist at: {}", pFeatureIdsPathValue.toString()));
    }
  }

  if(pGroupingTypeValue == GroupingType::FeaturesAndPhases)
  {
    if(auto* featurePhases = dataStructure.getDataAs<Int32Array>(pFeaturePhasesPathValue); featurePhases == nullptr)
    {
      return MakePreflightErrorResult(-27872, fmt::format("Feature Phases Array doesn't exist at: {}", pFeaturePhasesPathValue.toString()));
    }
  }

  if(pGroupingTypeValue == GroupingType::PartNumber)
  {
    if(auto* featureIds = dataStructure.getDataAs<Int32Array>(pPartNumberPathValue); featureIds == nullptr)
    {
      return MakePreflightErrorResult(-27874, fmt::format("Part Number Array doesn't exist at: {}", pPartNumberPathValue.toString()));
    }
  }

  // Return both the resultOutputActions via std::move()
  return {std::move(resultOutputActions)};
}

//------------------------------------------------------------------------------
Result<> WriteStlFileFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                         const std::atomic_bool& shouldCancel) const
{
  WriteStlFileInputValues inputValues;

  inputValues.GroupingType = filterArgs.value<ChoicesParameter::ValueType>(k_GroupingType_Key);
  inputValues.OutputStlFile = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputStlFile_Key);
  inputValues.OutputStlDirectory = filterArgs.value<FileSystemPathParameter::ValueType>(k_OutputStlDirectory_Key);
  inputValues.OutputStlPrefix = filterArgs.value<StringParameter::ValueType>(k_OutputStlPrefix_Key);
  inputValues.FeatureIdsPath = filterArgs.value<DataPath>(k_FeatureIdsPath_Key);
  inputValues.FeaturePhasesPath = filterArgs.value<DataPath>(k_FeaturePhasesPath_Key);
  inputValues.TriangleGeomPath = filterArgs.value<DataPath>(k_TriangleGeomPath_Key);
  inputValues.PartNumberPath = filterArgs.value<DataPath>(k_PartNumberPath_Key);

  return WriteStlFile(dataStructure, messageHandler, shouldCancel, &inputValues)();
}

namespace
{
namespace SIMPL
{
constexpr StringLiteral k_OutputStlDirectoryKey = "OutputStlDirectory";
constexpr StringLiteral k_OutputStlPrefixKey = "OutputStlPrefix";
constexpr StringLiteral k_SurfaceMeshFaceLabelsArrayPathKey = "SurfaceMeshFaceLabelsArrayPath";
} // namespace SIMPL
} // namespace

Result<Arguments> WriteStlFileFilter::FromSIMPLJson(const nlohmann::json& json)
{
  Arguments args = WriteStlFileFilter().getDefaultArguments();

  std::vector<Result<>> results;

  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::OutputFileFilterParameterConverter>(args, json, SIMPL::k_OutputStlDirectoryKey, k_OutputStlDirectory_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::StringFilterParameterConverter>(args, json, SIMPL::k_OutputStlPrefixKey, k_OutputStlPrefix_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataContainerSelectionFilterParameterConverter>(args, json, SIMPL::k_SurfaceMeshFaceLabelsArrayPathKey, k_TriangleGeomPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_SurfaceMeshFaceLabelsArrayPathKey, k_FeatureIdsPath_Key));

  Result<> conversionResult = MergeResults(std::move(results));

  return ConvertResultTo<Arguments>(std::move(conversionResult), std::move(args));
}
} // namespace nx::core
