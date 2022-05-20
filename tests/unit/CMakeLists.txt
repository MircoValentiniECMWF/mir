
# Setup cache directory
set(TEST_ECKIT_CACHE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cache/)
file(REMOVE_RECURSE ${TEST_ECKIT_CACHE_DIR})

foreach(_t
#    MIR-333
#    MIR-356
    MIR-425
    MIR-536
    PGEN-412
    area
    action_graph
    bounding_box
    gaussian_grid
    grib_encoding
    increments
    input_MultiDimensionalInput
    iterator
    knn_weighting
    spectral_order
    statistics
    style
    vector-space
    wind)
    ecbuild_add_test(
        TARGET            mir_tests_unit_${_t}
        SOURCES           ${_t}.cc
        LIBS              mir
        ENVIRONMENT       ${_testEnvironment}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()

ecbuild_add_executable(TARGET mir_tests_unit_grib_nearest SOURCES grib_nearest.cc LIBS mir NOINSTALL)

ecbuild_add_test(TARGET mir_tests_unit_grib_nearest_O32.grib1 COMMAND mir_tests_unit_grib_nearest ARGS O32.grib1 O32.yaml ENVIRONMENT ${_testEnvironment} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
ecbuild_add_test(TARGET mir_tests_unit_grib_nearest_O32.grib2 COMMAND mir_tests_unit_grib_nearest ARGS O32.grib2 O32.yaml ENVIRONMENT ${_testEnvironment} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(mir_HAVE_ATLAS)
    ecbuild_add_test(
        TARGET            mir_tests_unit_atlas
        SOURCES           atlas.cc
        LIBS              mir
        ENVIRONMENT       ${_testEnvironment}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

