#pragma once

#include "simplnx/simplnx_export.hpp"

#include "simplnx/Common/Result.hpp"
#include "simplnx/DataStructure/IDataArray.hpp"

namespace nx::core::HistogramUtilities
{
namespace serial
{
template <typename Type>
float32 CalculateIncrement(Type min, Type max, int32 numBins)
{
  return static_cast<float32>(max - min) / static_cast<float32>(numBins);
}

/**
 * @function FillBinRange
 * @brief This function fills a container that is STL compatible and has a bracket operator defined with the bin ranges in the following pattern:
 *  bin_ranges = {minimum, maximum, next maximum, ...} with the format being that the bin's range is defined by bin_ranges[bin_index] <= X < bin_ranges[bin_index + 1]
 * @tparam Type this the end type of the function in that the container and data values are of this type
 * @tparam Container this is the type of object the ranges are loaded into, !!! It is expected that this class is STL compatible nd has a defined `[]` operator !!!
 * @param outputContainer this is the object that the ranges will be loaded into. ASSUMPTION: size is >= numBins + 1 !!! NO Bounds Check!!!
 * @param rangeMinMax this is assumed to be the inclusive minimum value and exclusive maximum value for the overall histogram bins. FORMAT: [minimum, maximum)
 * @param numBins this is the total number of bin ranges being calculated and by extension the indexing value for the ranges
 * @param increment this is the uniform size of the bins
 */
template <typename Type, class Container>
void FillBinRanges(Container& outputContainer, const std::pair<Type, Type>& rangeMinMax, const int32 numBins, const float32 increment)
{
  // WARNING: No bounds checking for type compatibility, it is expected to be done higher up where the type is not abstracted
  // EXPECTED CONTAINER SIZE: numBins + 1

  if(numBins == 1) // if one bin, just set the range to the inputs
  {
    outputContainer[0] = rangeMinMax.first;
    outputContainer[1] = rangeMinMax.second;
    return;
  }

  // iterate through loading the middle values of the sequence considering `lower bound inclusive, upper bound exclusive`
  // We are filling a 2 component array. For STL containers this means the size of the container is 2*numbins
  // We are going to put all the lower bin values into a component and the upper bin values in another component
  for(int32 i = 0; i < numBins; i++)
  {
    outputContainer[i * 2 + 0] = rangeMinMax.first + static_cast<Type>(increment * i);
    outputContainer[i * 2 + 1] = rangeMinMax.first + static_cast<Type>(increment * (i + 1.0F));
  }
}

/**
 * @function FillBinRange
 * @brief This overload is provided in the case the bin size is not provided and therefore must be calculated - see above overload for more detail on functionality
 * @tparam Type this the end type of the function in that the container and data values are of this type
 * @tparam Container this is the type of object the ranges are loaded into, !!! It is expected that this class is STL compatible nd has a defined `[]` operator !!!
 * @param outputContainer this is the object that the ranges will be loaded into. ASSUMPTION: size is >= numBins + 1 !!! NO Bounds Check!!!
 * @param rangeMinMax this is assumed to be the inclusive minimum value and exclusive maximum value for the overall histogram bins. FORMAT: [minimum, maximum)
 * @param numBins this is the total number of bin ranges being calculated and by extension the indexing value for the ranges
 */
template <typename Type, class Container>
void FillBinRanges(Container& outputContainer, const std::pair<Type, Type>& rangeMinMax, const int32 numBins)
{
  // DEV NOTE: this function also serves to act as a jumping off point for implementing logarithmic histograms down the line

  // Uniform Bin Sizes
  const float32 increment = CalculateIncrement(rangeMinMax.first, rangeMinMax.second, numBins);

  FillBinRanges(outputContainer, rangeMinMax, numBins, increment);
}

template <typename Type>
auto CalculateBin(Type value, Type min, float32 increment)
{
  if constexpr(std::is_same_v<Type, bool>)
  {
    return static_cast<uint8>(std::floor(static_cast<float32>(static_cast<uint8>(value) - static_cast<uint8>(min)) / increment));
  }
  else
  {
    return static_cast<Type>(std::floor(static_cast<float32>(value - min) / increment));
  }
}

/**
 * @function GenerateHistogram
 * @brief This function creates a uniform histogram (logarithmic possible, but not currently implemented) it fills two arrays one with the ranges for each bin and one for bin counts
 * See FillBinRanges function for details on the high level structuring of the bin ranges array
 * @tparam Type this the end type of the function in that it is the scalar type of the input and by extension range data
 * @tparam SizeType this is the scalar type of the bin counts container
 * @tparam InputContainer this is the type of object the values are read from:
 * !!! In current implementation it is expected that this class is either AbstractDataStore or std::vector !!!
 * @tparam RangesContainer this is the type of object the ranges are stored/written to:
 * !!! In current implementation it is expected that this class is either AbstractDataStore or std::vector and whose scalar type matches Type !!!
 * @tparam CountsContainer this is the type of object the counts are stored/written to:
 * !!! In current implementation it is expected that this class is either AbstractDataStore or std::vector !!!
 * @param inputStore this is the container holding the data that will be binned
 * @param binRangesStore this is the object that the ranges will be loaded into.
 * @param rangeMinMax this is assumed to be the inclusive minimum value and exclusive maximum value for the overall histogram bins. FORMAT: [minimum, maximum)
 * @param shouldCancel this is an atomic value that will determine whether execution ends early; `true` cancels algorithm
 * @param numBins this is the total number of bin ranges being calculated and by extension the indexing value for the ranges
 * @param histogramCountsStore this is the container that will hold the counts for each bin (variable type sizing)
 * @param overflow this is an atomic counter for the number of values that fall outside the bin range
 */
template <typename Type, class InputContainer, class RangesContainer, class CountsContainer>
Result<> GenerateHistogram(const InputContainer& inputStore, RangesContainer& binRangesStore, const std::pair<Type, Type>& rangeMinMax, const std::atomic_bool& shouldCancel, const int32 numBins,
                           CountsContainer& histogramCountsStore, std::atomic<usize>& overflow)
{
  static_assert(std::is_same_v<typename InputContainer::value_type, typename RangesContainer::value_type>,
                "HistogramUtilities::GenerateHistogram: inputStore and binRangesStore must be of the same type. HistogramUtilities:99");

  if(binRangesStore.size() < numBins * 2)
  {
    return MakeErrorResult(-23761, fmt::format("HistogramUtilities::{}: binRangesStore is too small to hold ranges. Needed: {} | Current Size: {}. {}:{}", __func__, numBins + 1, binRangesStore.size(),
                                               __FILE__, __LINE__));
  }
  if(histogramCountsStore.size() < numBins)
  {
    return MakeErrorResult(-23762, fmt::format("HistogramUtilities::{}: histogramCountsStore is too small to hold counts. Needed: {} | Current Size: {}. {}:{}", __func__, numBins,
                                               histogramCountsStore.size(), __FILE__, __LINE__));
  }

  const float32 increment = CalculateIncrement(rangeMinMax.first, rangeMinMax.second, numBins);

  // Fill Bins
  FillBinRanges(binRangesStore, rangeMinMax, numBins, increment);

  for(usize i = 0; i < inputStore.size(); i++)
  {
    if(shouldCancel)
    {
      return MakeErrorResult(-23763, fmt::format("HistogramUtilities::{}: Signal Interrupt Received. {}:{}", __func__, __FILE__, __LINE__));
    }
    const auto bin = CalculateBin(inputStore[i], rangeMinMax.first, increment);
    if((bin >= 0) && (bin < numBins))
    {
      histogramCountsStore[bin]++;
    }
    else
    {
      overflow++;
    }
  }

  if(overflow > 0)
  {
    return MakeWarningVoidResult(-23764, fmt::format("HistogramUtilities::{}: Overflow detected: overflow count {}. {}:{}", __func__, overflow.load(), __FILE__, __LINE__));
  }

  return {};
}

/**
 * @function GenerateHistogram
 * @brief [Runs over specific component] This function creates a uniform histogram (logarithmic possible, but not currently implemented) it fills two arrays,
 * one with the ranges for each bin and one for bin counts
 * See FillBinRanges function for details on the high level structuring of the bin ranges array
 * @tparam Type this the end type of the function in that it is the scalar type of the input and by extension range data
 * @tparam RangesContainer this is the type of object the ranges are stored/written to:
 * !!! In current implementation it is expected that this class is either AbstractDataStore or std::vector and whose scalar type matches Type !!!
 * @tparam CountsContainer this is the type of object the counts are stored/written to:
 * !!! In current implementation it is expected that this class is either AbstractDataStore or std::vector !!!
 * @param inputStore this is the container holding the data that will be binned
 * @param binRangesStore this is the object that the ranges will be loaded into.
 * @param rangeMinMax this is assumed to be the inclusive minimum value and exclusive maximum value for the overall histogram bins. FORMAT: [minimum, maximum)
 * @param shouldCancel this is an atomic value that will determine whether execution ends early; `true` cancels algorithm
 * @param numBins this is the total number of bin ranges being calculated and by extension the indexing value for the ranges
 * @param histogramCountsStore this is the container that will hold the counts for each bin (variable type sizing)
 * @param overflow this is an atomic counter for the number of values that fall outside the bin range
 */
template <typename Type, class RangesContainer, class CountsContainer>
Result<> GenerateHistogramAtComponent(const AbstractDataStore<Type>& inputStore, RangesContainer& binRangesStore, const std::pair<Type, Type>& rangeMinMax, const std::atomic_bool& shouldCancel,
                                      const int32 numBins, CountsContainer& histogramCountsStore, std::atomic<usize>& overflow, usize componentIndex)
{
  static_assert(std::is_same_v<Type, typename RangesContainer::value_type>,
                "HistogramUtilities::GenerateHistogramAtComponent: inputStore and binRangesStore must be of the same type. HistogramUtilities:163");

  usize numComp = inputStore.getNumberOfComponents();
  if(componentIndex > numComp)
  {
    return MakeErrorResult(-23765, fmt::format("HistogramUtilities::{}: supplied component index is larger than component size of input array. Needed: x < {} | Currently: {}. {}:{}", __func__,
                                               numComp, componentIndex, __FILE__, __LINE__));
  }

  if(binRangesStore.size() < numBins + 1)
  {
    return MakeErrorResult(-23761, fmt::format("HistogramUtilities::{}: binRangesStore is too small to hold ranges. Needed: {} | Current Size: {}. {}:{}", __func__, numBins + 1, binRangesStore.size(),
                                               __FILE__, __LINE__));
  }
  if(histogramCountsStore.size() < numBins)
  {
    return MakeErrorResult(-23762, fmt::format("HistogramUtilities::{}: histogramCountsStore is too small to hold counts. Needed: {} | Current Size: {}. {}:{}", __func__, numBins,
                                               histogramCountsStore.size(), __FILE__, __LINE__));
  }

  const float32 increment = CalculateIncrement(rangeMinMax.first, rangeMinMax.second, numBins);

  // Fill Bins
  FillBinRanges(binRangesStore, rangeMinMax, numBins, increment);

  for(usize i = 0; i < inputStore.getNumberOfTuples(); i++)
  {
    if(shouldCancel)
    {
      return MakeErrorResult(-23763, fmt::format("HistogramUtilities::{}: Signal Interrupt Received. {}:{}", __func__, __FILE__, __LINE__));
    }
    const auto bin = CalculateBin(inputStore[i * numComp + componentIndex], rangeMinMax.first, increment);
    if((bin >= 0) && (bin < numBins))
    {
      histogramCountsStore[bin]++;
    }
    else
    {
      overflow++;
    }
  }

  if(overflow > 0)
  {
    return MakeWarningVoidResult(-23764, fmt::format("HistogramUtilities::{}: Overflow detected: overflow count {}. {}:{}", __func__, overflow.load(), __FILE__, __LINE__));
  }

  return {};
}

/**
 * @class GenerateHistogramFunctor
 * @brief This is a compatibility functor that leverages existing typecasting functions to execute GenerateHistogram() cleanly. In it there are two
 * definitions for the `()` operator that allows for implicit calculation of range, predicated whether a range is passed in or not
 */
struct GenerateHistogramFunctor
{
  template <typename Type, class... ArgsT>
  Result<> operator()(const IDataArray* inputArray, IDataArray* binRangesArray, ArgsT&&... args) const
  {
    const auto& inputStore = inputArray->template getIDataStoreRefAs<AbstractDataStore<Type>>();

    auto minMax = std::minmax_element(inputStore.begin(), inputStore.end());

    return GenerateHistogram(inputStore, binRangesArray->template getIDataStoreRefAs<AbstractDataStore<Type>>(), std::make_pair(*minMax.first, *minMax.second + static_cast<Type>(1.0)),
                             std::forward<ArgsT>(args)...);
  }

  template <typename Type, class... ArgsT>
  Result<> operator()(const IDataArray* inputArray, IDataArray* binRangesArray, std::pair<float64, float64>&& rangeMinMax, ArgsT&&... args) const
  {
    const auto& inputStore = inputArray->template getIDataStoreRefAs<AbstractDataStore<Type>>();

    // check range ordering : should be min, max
    if(rangeMinMax.first > rangeMinMax.second)
    {
      return MakeErrorResult(-23760, fmt::format("GenerateHistogramFunctor::{}: The range min value is larger than the max value. Min value: {} | Max Value: {}. {}:{}", __func__, rangeMinMax.first,
                                                 rangeMinMax.second, __FILE__, __LINE__));
    }

    return GenerateHistogram(inputStore, binRangesArray->template getIDataStoreRefAs<AbstractDataStore<Type>>(),
                             std::make_pair(static_cast<Type>(rangeMinMax.first), static_cast<Type>(rangeMinMax.second)), std::forward<ArgsT>(args)...);
  }
};
} // namespace serial

namespace concurrent
{
/**
 * @class GenerateHistogramImpl
 * @brief This class is a pseudo-wrapper for the serial::GenerateHistogram, the reason for this class' existence is to hold/define ownership of objects in each thread
 * @tparam Type this the end type of the function in that the container and data values are of this type
 * @tparam SizeType this is the scalar type of the bin counts container
 */
template <typename Type, std::integral SizeType>
class GenerateHistogramImpl
{
public:
  /**
   * @function constructor
   * @brief This constructor requires a defined range and creates the object
   * @param inputStore this is the AbstractDataStore holding the data that will be binned
   * @param binRangesStore this is the AbstractDataStore that the ranges will be loaded into.
   * @param rangeMinMax this is assumed to be the inclusive minimum value and exclusive maximum value for the overall histogram bins. FORMAT: [minimum, maximum)
   * @param shouldCancel this is an atomic value that will determine whether execution ends early
   * @param numBins this is the total number of bin ranges being calculated and by extension the indexing value for the ranges
   * @param histogramStore this is the AbstractDataStore that will hold the counts for each bin (variable type sizing)
   * @param overflow this is an atomic counter for the number of values that fall outside the bin range
   */
  GenerateHistogramImpl(const AbstractDataStore<Type>& inputStore, AbstractDataStore<Type>& binRangesStore, std::pair<float64, float64>&& rangeMinMax, const std::atomic_bool& shouldCancel,
                        const int32 numBins, AbstractDataStore<SizeType>& histogramStore, std::atomic<usize>& overflow)
  : m_InputStore(inputStore)
  , m_ShouldCancel(shouldCancel)
  , m_NumBins(numBins)
  , m_BinRangesStore(binRangesStore)
  , m_HistogramStore(histogramStore)
  , m_Overflow(overflow)
  {
    m_Range = std::make_pair(static_cast<Type>(rangeMinMax.first), static_cast<Type>(rangeMinMax.second));
  }

  /**
   * @function constructor
   * @brief This constructor constructs the object then calculates and stores the range implicitly
   * @param inputStore this is the AbstractDataStore holding the data that will be binned
   * @param binRangesStore this is the AbstractDataStore that the ranges will be loaded into.
   * @param shouldCancel this is an atomic value that will determine whether execution ends early
   * @param numBins this is the total number of bin ranges being calculated and by extension the indexing value for the ranges
   * @param histogramStore this is the AbstractDataStore that will hold the counts for each bin (variable type sizing)
   * @param overflow this is an atomic counter for the number of values that fall outside the bin range
   */
  GenerateHistogramImpl(const AbstractDataStore<Type>& inputStore, AbstractDataStore<Type>& binRangesStore, const std::atomic_bool& shouldCancel, const int32 numBins,
                        AbstractDataStore<SizeType>& histogramStore, std::atomic<usize>& overflow)
  : m_InputStore(inputStore)
  , m_ShouldCancel(shouldCancel)
  , m_NumBins(numBins)
  , m_BinRangesStore(binRangesStore)
  , m_HistogramStore(histogramStore)
  , m_Overflow(overflow)
  {
    auto minMax = std::minmax_element(m_InputStore.begin(), m_InputStore.end());
    m_Range = std::make_pair(*minMax.first, *minMax.second + static_cast<Type>(1.0));
  }

  ~GenerateHistogramImpl() = default;

  /**
   * @function operator()
   * @brief This function serves as the execute method
   */
  void operator()() const
  {
    serial::GenerateHistogram(m_InputStore, m_BinRangesStore, m_Range, m_ShouldCancel, m_NumBins, m_HistogramStore, m_Overflow);
  }

private:
  const std::atomic_bool& m_ShouldCancel;
  const int32 m_NumBins = 1;
  std::pair<Type, Type> m_Range = {static_cast<Type>(0.0), static_cast<Type>(0.0)};
  const AbstractDataStore<Type>& m_InputStore;
  AbstractDataStore<Type>& m_BinRangesStore;
  AbstractDataStore<SizeType>& m_HistogramStore;
  std::atomic<usize>& m_Overflow;
};

/**
 * @class InstantiateHistogramImplFunctor
 * @brief This is a compatibility functor that leverages existing typecasting functions to create the appropriately typed GenerateHistogramImpl() cleanly.
 * Designed for compatibility with the existing parallel execution classes.
 */
struct InstantiateHistogramImplFunctor
{
  template <typename T, class... ArgsT>
  auto operator()(const IDataArray* inputArray, IDataArray* binRangesArray, ArgsT&&... args)
  {
    return GenerateHistogramImpl(inputArray->template getIDataStoreRefAs<AbstractDataStore<T>>(), binRangesArray->template getIDataStoreRefAs<AbstractDataStore<T>>(), std::forward<ArgsT>(args)...);
  }
};
} // namespace concurrent
} // namespace nx::core::HistogramUtilities
