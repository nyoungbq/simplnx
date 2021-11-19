#include "ArrayThresholdsParameter.hpp"

#include <fmt/core.h>

#include <nlohmann/json.hpp>

namespace complex
{
ArrayThresholdsParameter::ArrayThresholdsParameter(const std::string& name, const std::string& humanName, const std::string& helpText, const ValueType& defaultValue)
: MutableDataParameter(name, humanName, helpText, Category::Created)
, m_DefaultValue(defaultValue)
{
}

Uuid ArrayThresholdsParameter::uuid() const
{
  return ParameterTraits<ArrayThresholdsParameter>::uuid;
}

IParameter::AcceptedTypes ArrayThresholdsParameter::acceptedTypes() const
{
  return {typeid(ValueType)};
}

nlohmann::json ArrayThresholdsParameter::toJson(const std::any& value) const
{
  auto thresholds = std::any_cast<ValueType>(value);
  nlohmann::json json = thresholds.toJson();
  return json;
}

Result<std::any> ArrayThresholdsParameter::fromJson(const nlohmann::json& json) const
{
  if(!json.is_string())
  {
    return MakeErrorResult<std::any>(-2, fmt::format("JSON value for key \"{}\" is not a string", name()));
  }
  auto string = json.get<std::string>();
  std::optional<DataPath> path = DataPath::FromString(string);
  if(!path.has_value())
  {
    return MakeErrorResult<std::any>(-3, fmt::format("Failed to parse \"{}\" as DataPath", string));
  }
  return {std::move(*path)};
}

IParameter::UniquePointer ArrayThresholdsParameter::clone() const
{
  return std::make_unique<ArrayThresholdsParameter>(name(), humanName(), helpText(), m_DefaultValue);
}

std::any ArrayThresholdsParameter::defaultValue() const
{
  return defaultPath();
}

typename ArrayThresholdsParameter::ValueType ArrayThresholdsParameter::defaultPath() const
{
  return m_DefaultValue;
}

Result<> ArrayThresholdsParameter::validate(const DataStructure& dataStructure, const std::any& value) const
{
  auto threshold = std::any_cast<ValueType>(value);

  return validatePaths(dataStructure, threshold);
}

Result<> ArrayThresholdsParameter::validatePath(const DataStructure& dataStructure, const DataPath& value) const
{
  if(value.empty())
  {
    return {nonstd::make_unexpected(std::vector<Error>{{-1, "DataPath cannot be empty"}})};
  }
  const DataObject* object = dataStructure.getData(value);
  if(object != nullptr)
  {
    return {nonstd::make_unexpected(std::vector<Error>{{-2, fmt::format("Object already exists at path \"{}\"", value.toString())}})};
  }

  return {};
}

Result<> ArrayThresholdsParameter::validatePaths(const DataStructure& dataStructure, const ValueType& value) const
{
  auto paths = value.getRequiredPaths();
  for(const auto& path : paths)
  {
    auto validation = validatePath(dataStructure, path);
    if(validation.invalid())
    {
      return validation;
    }
  }

  return {};
}

Result<std::any> ArrayThresholdsParameter::resolve(DataStructure& dataStructure, const std::any& value) const
{
  return {};
}
} // namespace complex