vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO tat/mimetic
    REF ebf95dffadc2fbe1bb238abdb92dfc9067591ca7
    SHA512 33df3a629bff0ead930777ca48734a0f8e36998ef598ba369598ec43b77ed33167e528914137b07176fbf95964b540da6126aa82654645bd0ee5e4bc08b3bcd2
    HEAD_REF master
)

vcpkg_list(SET OPTIONS)

if(VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
    list(APPEND OPTIONS
        --disable-static
        --enable-shared
        --disable-test
    )
else()
    list(APPEND OPTIONS
        --enable-static
        --disable-shared
        --disable-test
    )
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/Makefile.am ${SOURCE_PATH} COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/Makefile.in ${SOURCE_PATH} COPYONLY)

vcpkg_configure_make(
    SOURCE_PATH "${SOURCE_PATH}"
    DETERMINE_BUILD_TRIPLET
    NO_ADDITIONAL_PATHS
    OPTIONS
        ${OPTIONS}
)


vcpkg_install_make()

vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

#vcpkg_cmake_install()
#vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/charls)
#file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")

#vcpkg_copy_pdbs()

#vcpkg_fixup_pkgconfig()
