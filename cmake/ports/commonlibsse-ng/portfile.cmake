# Get port from Github
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO alandtse/CommonLibVR
    REF 8e42371362b17770a6ad389f48f4a6fdca998e55
    SHA512 c44350768ab4a35993ca99b966c2f912616655fc63df21177e73c092654a0266446845f67adc1cc2cc179b0c5c5e077622703737a4b5820e647bda8b5fd00b32
    HEAD_REF ng
)

# Get submodule and copy to extern/ folder (done manually because Vcpkg does not support Git submodules)
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH_OPENVR
    REPO ValveSoftware/openvr
    REF ebdea152f8aac77e9a6db29682b81d762159df7e
    SHA512 4fb668d933ac5b73eb4e97eb29816176e500a4eaebe2480cd0411c95edfb713d58312036f15db50884a2ef5f4ca44859e108dec2b982af9163cefcfc02531f63
    HEAD_REF master
)
 file(COPY "${SOURCE_PATH_OPENVR}/" DESTINATION "${SOURCE_PATH}/extern/openvr")
 file(REMOVE_RECURSE "${SOURCE_PATH_OPENVR}/")

 # Configure options to build
vcpkg_configure_cmake(
        SOURCE_PATH "${SOURCE_PATH}"
        PREFER_NINJA
        OPTIONS -DBUILD_TESTS=off -DSKSE_SUPPORT_XBYAK=on
)

vcpkg_install_cmake()
vcpkg_cmake_config_fixup(PACKAGE_NAME CommonLibSSE CONFIG_PATH lib/cmake)
vcpkg_copy_pdbs()

file(INSTALL "${SOURCE_PATH}/extern/openvr/headers/openvr.h" DESTINATION ${CURRENT_PACKAGES_DIR}/include)
file(GLOB CMAKE_CONFIGS "${CURRENT_PACKAGES_DIR}/share/CommonLibSSE/CommonLibSSE/*.cmake")
file(INSTALL ${CMAKE_CONFIGS} DESTINATION "${CURRENT_PACKAGES_DIR}/share/CommonLibSSE")
file(INSTALL "${SOURCE_PATH}/cmake/CommonLibSSE.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/CommonLibSSE")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/share/CommonLibSSE/CommonLibSSE")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
