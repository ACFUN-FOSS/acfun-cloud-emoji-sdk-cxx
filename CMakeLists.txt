cmake_minimum_required(VERSION 3.22)
project(acfun_cloud_emoji_sdk_cxx LANGUAGES CXX VERSION 1.0.0)
cmake_policy(SET CMP0003 NEW)
include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_TEST ${PROJECT_IS_TOP_LEVEL})
set(INSTALL_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR}/acfun_foss/acfun_cloud_emoji_sdk_cxx)


####################
# Dependencies
####################
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(reflectcpp CONFIG REQUIRED)
find_package(concurrencpp CONFIG REQUIRED)

if(BUILD_TEST)
    find_package(GTest CONFIG REQUIRED)
endif()

####################
# Targets
####################
add_library(acfun_cloud_emoji_sdk_cxx "acfun_cloud_emoji.cxx" "acfun_cloud_emoji.hxx")
target_compile_options(acfun_cloud_emoji_sdk_cxx PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
generate_export_header(acfun_cloud_emoji_sdk_cxx EXPORT_MACRO_NAME ACEAPI)
target_include_directories(acfun_cloud_emoji_sdk_cxx PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
)
target_link_libraries(acfun_cloud_emoji_sdk_cxx
    PUBLIC
        concurrencpp::concurrencpp
    PRIVATE
        cpr::cpr
        nlohmann_json::nlohmann_json
        reflectcpp::reflectcpp
)

if(BUILD_TEST)
    add_executable(test test.cxx)
    target_compile_options(test PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    target_link_libraries(test PRIVATE acfun_cloud_emoji_sdk_cxx GTest::gtest)
endif()

####################
# Make package config script
####################
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/acfun_cloud_emoji_sdk_cxxConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/acfun_cloud_emoji_sdk_cxxConfig.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/acfun_cloud_emoji_sdk_cxxConfig.cmake"
   INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AcfunCloudEmoji
)


####################
# Install
####################
# install libraries
install(TARGETS acfun_cloud_emoji_sdk_cxx
		EXPORT acfun_cloud_emoji_sdk_cxxTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# install headers
install(FILES
	acfun_cloud_emoji.hxx
	${CMAKE_CURRENT_BINARY_DIR}/acfun_cloud_emoji_sdk_cxx_export.h
	DESTINATION
	${INSTALL_INC_DIR}
)

# install package config scripts
install(
    EXPORT acfun_cloud_emoji_sdk_cxxTargets
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/acfun_cloud_emoji_sdk_cxx
	NAMESPACE acfunfoss::
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/acfun_cloud_emoji_sdk_cxxConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/acfun_cloud_emoji_sdk_cxxConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/acfun_cloud_emoji_sdk_cxx
)
