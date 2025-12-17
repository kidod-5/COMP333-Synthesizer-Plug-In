cmake_minimum_required(VERSION 3.24)
set(CMAKE_OSX_ARCHITECTURES "arm64")
project(MLPlugIn VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# For libtorch
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libtorch")
find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# CPM to bring in JUCE
set(CPM_DOWNLOAD_VERSION 0.34.0)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if (NOT EXISTS ${CPM_DOWNLOAD_LOCATION})
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif ()
include(${CPM_DOWNLOAD_LOCATION})

# Bring in JUCE
CPMAddPackage(
    NAME juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG origin/master
)

# Source files
set(SourceFiles
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/AudioGenerators/Noise.h
        Source/RAVE/test_libtorch.cpp
        Source/RAVE/test_libtorch.h
	    Source/RAVE/rave.cpp
	    Source/RAVE/rave.h
	    Source/RAVE/RaveModelManager.cpp
	    Source/RAVE/RaveModelManager.h
	    Source/RAVE/BlockTypes.h
	    Source/RAVE/FIFO.h
)

# Add plugin
juce_add_plugin(${PROJECT_NAME}
        COMPANY_NAME dynamicsounds
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        JUCE_VST3_CAN_REPLACE_VST2 FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Tap1
        PLUGIN_CODE Reg0
        FORMATS VST3 AU Standalone
        PRODUCT_NAME "MLPlugIn"
)

# Source files in IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})
target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

# Compile definitions
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Link JUCE + libtorch
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        juce::juce_analytics
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_core
        juce::juce_data_structures
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        ${TORCH_LIBRARIES}
)

# Runtime output directory
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# -------------------------------
# macOS: Copy libtorch dylibs into plugin bundles
# -------------------------------
if(APPLE)
    file(GLOB TORCH_DYLIBS
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libtorch/lib/libtorch*.dylib"
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libtorch/lib/libc10*.dylib"
    )

    foreach(TGT IN ITEMS
            ${PROJECT_NAME}_AU
            ${PROJECT_NAME}_VST3
            ${PROJECT_NAME}_Standalone
        )
        add_custom_command(TARGET ${TGT} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${TGT}>/../Frameworks"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${TORCH_DYLIBS}
                "$<TARGET_FILE_DIR:${TGT}>/../Frameworks"
        )
    endforeach()
endif()
