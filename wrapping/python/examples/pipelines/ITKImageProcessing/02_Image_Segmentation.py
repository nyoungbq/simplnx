import simplnx as nx

import itkimageprocessing as cxitk
import orientationanalysis as cxor
import simplnx_test_dirs as nxtest

import numpy as np

# Create a Data Structure
data_structure = nx.DataStructure()

# Filter 1

generated_file_list_value = nx.GeneratedFileListParameter.ValueType()
generated_file_list_value.input_path = str(nxtest.get_data_directory() / "Porosity_Image/")
generated_file_list_value.ordering = nx.GeneratedFileListParameter.Ordering.LowToHigh

generated_file_list_value.file_prefix = "slice_"
generated_file_list_value.file_suffix = ""
generated_file_list_value.file_extension = ".tif"
generated_file_list_value.start_index = 11
generated_file_list_value.end_index = 174
generated_file_list_value.increment_index = 1
generated_file_list_value.padding_digits = 2

# Instantiate Filter
nx_filter = cxitk.ITKImportImageStackFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    cell_attribute_matrix_name="Cell Data",
    image_data_array_name="ImageData",
    output_image_geometry_path=nx.DataPath("ImageDataContainer"),
    image_transform_index=0,
    input_file_list_object =generated_file_list_value,
    origin=[0.0, 0.0, 0.0],
    spacing=[1.0, 1.0, 1.0]
)
nxtest.check_filter_result(nx_filter, result)

# Filter 2
# Set Up Thresholds and Instantiate Filter
threshold_1 = nx.ArrayThreshold()
threshold_1.array_path = nx.DataPath("ImageDataContainer/Cell Data/ImageData")
threshold_1.comparison = nx.ArrayThreshold.ComparisonType.Equal
threshold_1.value = 0.0

threshold_set = nx.ArrayThresholdSet()
threshold_set.thresholds = [threshold_1]
dt = nx.DataType.boolean  # This line specifies the DataType for the threshold

# Instantiate Filter
nx_filter = nx.MultiThresholdObjectsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    array_thresholds_object=threshold_set,
    output_data_array_name="Mask",
    created_mask_type=dt,  # Use the DataType variable here
    use_custom_false_value=False,
    use_custom_true_value=False
    # custom_false_value: float = ...,  # Not currently part of the code
    # custom_true_value: float = ...,  # Not currently part of the code
)
nxtest.check_filter_result(nx_filter, result)


# Filter 3
# Instantiate Filter
nx_filter = nx.ScalarSegmentFeaturesFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    active_array_name="Active",
    cell_feature_group_name="Cell Feature Data",
    feature_ids_name="FeatureIds",
    input_image_geometry_path=nx.DataPath("ImageDataContainer"),
    input_array_path=nx.DataPath("ImageDataContainer/Cell Data/ImageData"),
    mask_path=nx.DataPath("ImageDataContainer/Cell Data/Mask"),
    randomize_features=True,
    scalar_tolerance=0,
    use_mask=True
)
nxtest.check_filter_result(nx_filter, result)

# Filter 4
# Instantiate Filter
nx_filter = nx.ComputeFeatureSizesFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    equivalent_diameters_name="EquivalentDiameters",
    feature_attribute_matrix_path =nx.DataPath("ImageDataContainer/Cell Feature Data"),
    feature_ids_path=nx.DataPath("ImageDataContainer/Cell Data/FeatureIds"),
    input_image_geometry_path =nx.DataPath("ImageDataContainer"),
    num_elements_name="NumElements",
    save_element_sizes=False,
    volumes_name="Volumes"
)
nxtest.check_filter_result(nx_filter, result)

# Filter 5
# Instantiate Filter
nx_filter = nx.CopyFeatureArrayToElementArrayFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    created_array_suffix="Created Array Suffix",
    feature_ids_path=nx.DataPath("ImageDataContainer/Cell Data/FeatureIds"),
    selected_feature_array_paths=[nx.DataPath("ImageDataContainer/Cell Feature Data/EquivalentDiameters")]
)
nxtest.check_filter_result(nx_filter, result)

# Filter 6
# Instantiate Filter
nx_filter = nx.CreateDataArrayFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    set_tuple_dimensions=False,
    component_count=1,
    data_format="",
    initialization_value_str="1",
    numeric_type_index=nx.NumericType.int32,
    output_array_path=nx.DataPath("ImageDataContainer/Cell Data/Phases")
    # tuple_dimensions: List[List[float]] = ...  # Not currently part of the code
)
nxtest.check_filter_result(nx_filter, result)

# Filter 7
# Instantiate Filter
nx_filter = nx.ConditionalSetValueFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    conditional_array_path=nx.DataPath("ImageDataContainer/Cell Data/Mask"),
    invert_mask=False,
    replace_value="2",
    selected_array_path=nx.DataPath("ImageDataContainer/Cell Data/Phases"),
    use_conditional=True
    # remove_value: str = ...,  # Not currently part of the code
)
nxtest.check_filter_result(nx_filter, result)

# Filter 8
# Instantiate Filter
nx_filter = nx.ComputeFeaturePhasesFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    cell_features_attribute_matrix_path=nx.DataPath("ImageDataContainer/Cell Feature Data"),
    cell_phases_array_path=nx.DataPath("ImageDataContainer/Cell Data/Phases"),
    feature_ids_path=nx.DataPath("ImageDataContainer/Cell Data/FeatureIds"),
    feature_phases_array_name="Phases"
)
nxtest.check_filter_result(nx_filter, result)
# Filter 9

# Instantiate Filter
nx_filter = nx.ComputeFeatureCentroidsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    centroids_array_name="Centroids",
    feature_attribute_matrix_path =nx.DataPath("ImageDataContainer/Cell Feature Data"),
    feature_ids_path=nx.DataPath("ImageDataContainer/Cell Data/FeatureIds"),
    input_image_geometry_path=nx.DataPath("ImageDataContainer")
)
nxtest.check_filter_result(nx_filter, result)

# Filter 10
# Instantiate Filter
nx_filter = nx.CreateAttributeMatrixFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    data_object_path=nx.DataPath("Ensemble AttributeMatrix"),
    tuple_dimensions=[[3.0]]
)
nxtest.check_filter_result(nx_filter, result)

# Filter 11
# Instantiate Filter
nx_filter = nx.WriteDREAM3DFilter()
# Execute Filter with Parameters
output_file_path = nxtest.get_data_directory() / "Output/ImagesStack/Images.dream3d"
result = nx_filter.execute(data_structure=data_structure, 
                                        export_file_path=output_file_path, 
                                        write_xdmf_file=True)
nxtest.check_filter_result(nx_filter, result)

# *****************************************************************************
# THIS SECTION IS ONLY HERE FOR CLEANING UP THE CI Machines
# If you are using this code, you should COMMENT out the next line
nxtest.cleanup_test_file(output_file_path)
# *****************************************************************************

print("===> Pipeline Complete")

