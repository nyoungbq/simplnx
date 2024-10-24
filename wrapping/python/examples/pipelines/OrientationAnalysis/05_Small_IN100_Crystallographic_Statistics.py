import simplnx as nx

import itkimageprocessing as cxitk
import orientationanalysis as cxor
import simplnx_test_dirs as nxtest

import numpy as np

# Create a Data Structure
data_structure = nx.DataStructure()

# Filter 1
# Instantiate Import Data Parameter
import_data = nx.Dream3dImportParameter.ImportData()
import_data.file_path = str(nxtest.get_data_directory() / "Output/Statistics/SmallIN100_Morph.dream3d")
import_data.data_paths = None
# Instantiate Filter
nx_filter = nx.ReadDREAM3DFilter()
# Execute Filter with Parameters
result = nx_filter.execute(data_structure=data_structure, import_data_object=import_data)
nxtest.check_filter_result(nx_filter, result)

# Filter 2
# Instantiate Filter
nx_filter = nx.DeleteDataFilter()
# Execute Filter With Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    removed_data_path=[nx.DataPath("DataContainer/Cell Feature Data/AvgQuats"),
                       nx.DataPath("DataContainer/Cell Feature Data/AvgEulerAngles")]

)
nxtest.check_filter_result(nx_filter, result)

# Filter 3
# Instantiate and Execute Filter
# Note: This filter might need additional parameters depending on the intended data removal.
nx_filter = nx.DeleteDataFilter()
result = nx_filter.execute(
    data_structure=data_structure
    # removed_data_path: List[DataPath] = ...  # Not currently part of the code
)
nxtest.check_filter_result(nx_filter, result)

# Filter 4
# Instantiate Filter
nx_filter = cxor.ComputeAvgOrientationsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    avg_euler_angles_array_name="AvgEulerAngles",
    avg_quats_array_name="AvgQuats",
    cell_feature_attribute_matrix_path=nx.DataPath("DataContainer/Cell Feature Data"),
    cell_feature_ids_array_path=nx.DataPath("DataContainer/Cell Data/FeatureIds"),
    cell_phases_array_path=nx.DataPath("DataContainer/Cell Data/Phases"),
    cell_quats_array_path=nx.DataPath("DataContainer/Cell Data/Quats"),
    crystal_structures_array_path=nx.DataPath("DataContainer/Cell Ensemble Data/CrystalStructures")
)
nxtest.check_filter_result(nx_filter, result)

# Filter 5
# Instantiate Filter
nx_filter = cxor.ComputeMisorientationsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    avg_quats_array_path=nx.DataPath("DataContainer/Cell Feature Data/AvgQuats"),
    crystal_structures_array_path=nx.DataPath("DataContainer/Cell Ensemble Data/CrystalStructures"),
    feature_phases_array_path=nx.DataPath("DataContainer/Cell Feature Data/Phases"),
    compute_avg_misors=False,
    misorientation_list_array_name="MisorientationList",
    neighbor_list_array_path=nx.DataPath("DataContainer/Cell Feature Data/NeighborhoodList")
    # avg_misorientations_array_name: str = ...,  # Not currently part of the code
)
nxtest.check_filter_result(nx_filter, result)

# Filter 6
# Instantiate Filter
nx_filter = cxor.ComputeSchmidsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    avg_quats_array_path=nx.DataPath("DataContainer/Cell Feature Data/AvgQuats"),
    crystal_structures_array_path=nx.DataPath("DataContainer/Cell Ensemble Data/CrystalStructures"),
    feature_phases_array_path=nx.DataPath("DataContainer/Cell Feature Data/Phases"),
    loading_direction=[1.0, 1.0, 1.0],
    override_system=False,
    poles_array_name="Poles",
    schmids_array_name="Schmids",
    slip_systems_array_name="SlipSystems",
    store_angle_components=False
    # lambdas_array_name: str = ...,  # Not currently part of the code
    # phis_array_name: str = ...,  # Not currently part of the code
    # slip_direction: List[float] = ...,  # Not currently part of the code
    # slip_plane: List[float] = ...,  # Not currently part of the code
)
nxtest.check_filter_result(nx_filter, result)

# Filter 7
# Instantiate Filter
nx_filter = cxor.ComputeFeatureReferenceMisorientationsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    avg_quats_array_path=nx.DataPath("DataContainer/Cell Feature Data/AvgQuats"),
    cell_phases_array_path=nx.DataPath("DataContainer/Cell Data/Phases"),
    crystal_structures_array_path=nx.DataPath("DataContainer/Cell Ensemble Data/CrystalStructures"),
    feature_avg_misorientations_array_name="FeatureAvgMisorientations",
    feature_ids_path=nx.DataPath("DataContainer/Cell Data/FeatureIds"),
    feature_reference_misorientations_array_name="FeatureReferenceMisorientations",
    quats_array_path=nx.DataPath("DataContainer/Cell Data/Quats"),
    reference_orientation_index=0
    # cell_feature_attribute_matrix_path=nx.DataPath("DataContainer/"),  # Not currently part of the code
    # g_beuclidean_distances_array_path=nx.DataPath("DataContainer/"),  # Not currently part of the code
)
nxtest.check_filter_result(nx_filter, result)

# Filter 8
# Instantiate Filter
nx_filter = cxor.ComputeKernelAvgMisorientationsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    cell_phases_array_path=nx.DataPath("DataContainer/Cell Data/Phases"),
    crystal_structures_array_path=nx.DataPath("DataContainer/Cell Ensemble Data/CrystalStructures"),
    feature_ids_path=nx.DataPath("DataContainer/Cell Data/FeatureIds"),
    kernel_average_misorientations_array_name="KernelAverageMisorientations",
    kernel_size=[1, 1, 1],
    quats_array_path=nx.DataPath("DataContainer/Cell Data/Quats"),
    input_image_geometry_path=nx.DataPath("DataContainer")
)
nxtest.check_filter_result(nx_filter, result)

# Filter 9
# Instantiate Filter
nx_filter = nx.WriteDREAM3DFilter()
# Set Output File Path
output_file_path =  nxtest.get_data_directory() / "Output/Statistics/SmallIN100_CrystalStats.dream3d"
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure, 
    export_file_path=output_file_path, 
    write_xdmf_file=True
)
nxtest.check_filter_result(nx_filter, result)

# *****************************************************************************
# THIS SECTION IS ONLY HERE FOR CLEANING UP THE CI Machines
# If you are using this code, you should COMMENT out the next line
nxtest.cleanup_test_file(import_data.file_path)
# *****************************************************************************

print("===> Pipeline Complete")
