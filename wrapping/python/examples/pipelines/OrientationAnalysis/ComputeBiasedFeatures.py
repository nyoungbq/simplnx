import simplnx as nx

import itkimageprocessing as cxitk
import orientationanalysis as cxor
import simplnx_test_dirs as nxtest

import numpy as np

#Create a Data Structure
data_structure = nx.DataStructure()

# Filter 1
# Instantiate Import Data
import_data = nx.Dream3dImportParameter.ImportData()
import_data.file_path = str(nxtest.get_data_directory() / "Output/Statistics/SmallIN100_CrystalStats.dream3d")
import_data.data_paths = None
# Instantiate Filter
nx_filter = nx.ReadDREAM3DFilter()
# Execute Filter with Parameters
result = nx_filter.execute(data_structure=data_structure,
                        import_data_object=import_data
)
nxtest.check_filter_result(nx_filter, result)

# Filter 2
# Instantiate Filter
nx_filter = nx.DeleteDataFilter()
# Execute Filter With Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    removed_data_path=[nx.DataPath("DataContainer/Cell Feature Data/Centroids"),
                       nx.DataPath("DataContainer/Cell Feature Data/EquivalentDiameters"),
                       nx.DataPath("DataContainer/Cell Feature Data/NumElements"),
                       nx.DataPath("DataContainer/Cell Feature Data/Omega3s"),
                       nx.DataPath("DataContainer/Cell Feature Data/AxisLengths"),
                       nx.DataPath("DataContainer/Cell Feature Data/AxisEulerAngles"),
                       nx.DataPath("DataContainer/Cell Feature Data/AspectRatios"),
                       nx.DataPath("DataContainer/Cell Feature Data/Shape Volumes"),
                       nx.DataPath("DataContainer/Cell Feature Data/NumNeighbors"),
                       nx.DataPath("DataContainer/Cell Feature Data/NeighborList"),
                       nx.DataPath("DataContainer/Cell Feature Data/SharedSurfaceAreaList"),
                       nx.DataPath("DataContainer/Cell Feature Data/NeighborhoodList"),
                       nx.DataPath("DataContainer/Cell Feature Data/SurfaceAreaVolumeRatio"),
                       nx.DataPath("DataContainer/Cell Feature Data/AvgQuats"),
                       nx.DataPath("DataContainer/Cell Feature Data/AvgEulerAngles")]
)
nxtest.check_filter_result(nx_filter, result)


# Filter 2
# Instantiate Filter
nx_filter = nx.ComputeFeatureNeighborsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    #boundary_cells: str = ...,
    cell_feature_array_path=nx.DataPath("DataContainer/Cell Feature Data"),
    feature_ids_path=nx.DataPath("DataContainer/Cell Data/FeatureIds"),
    input_image_geometry_path =nx.DataPath("DataContainer"),
    neighbor_list_name="NeighborList",
    number_of_neighbors_name="NumNeighbors",
    shared_surface_area_list_name="SharedSurfaceAreaList",
    store_boundary_cells=False,
    store_surface_features=False
    #surface_features: str = ...
)
nxtest.check_filter_result(nx_filter, result)

# Filter 3
# Instantiate Filter
nx_filter = nx.ComputeFeatureCentroidsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    centroids_array_name="Centroids",
    feature_attribute_matrix_path=nx.DataPath("DataContainer/Cell Feature Data"),
    feature_ids_path=nx.DataPath("DataContainer/Cell Data/FeatureIds"),
    input_image_geometry_path=nx.DataPath("DataContainer")
)
nxtest.check_filter_result(nx_filter, result)

# Filter 4
# Instantiate Filter
nx_filter = nx.ComputeSurfaceFeaturesFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    feature_attribute_matrix_path=nx.DataPath("DataContainer/Cell Feature Data"),
    input_image_geometry_path=nx.DataPath("DataContainer"),
    feature_ids_path=nx.DataPath("DataContainer/Cell Data/FeatureIds"),
    mark_feature_0_neighbors=True,
    surface_features_array_name="SurfaceFeatures"
)
nxtest.check_filter_result(nx_filter, result)

# Filter 5
# Instantiate Filter
nx_filter = nx.ComputeBiasedFeaturesFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    biased_features_array_name="BiasedFeatures",
    calc_by_phase=True,
    centroids_array_path=nx.DataPath("DataContainer/Cell Feature Data/Centroids"),
    input_image_geometry_path=nx.DataPath("DataContainer"),
    phases_array_path=nx.DataPath("DataContainer/Cell Feature Data/Phases"),
    surface_features_array_path=nx.DataPath("DataContainer/Cell Feature Data/SurfaceFeatures")
)
nxtest.check_filter_result(nx_filter, result)

# Filter 6
# Output file path for Filter 4
output_file_path = nxtest.get_data_directory() / "Output/ComputeBiasedFeatures/ComputeBiasedFeatures.dream3d"
# Instantiate Filter
nx_filter = nx.WriteDREAM3DFilter()
# Execute Filter with Parameters
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
