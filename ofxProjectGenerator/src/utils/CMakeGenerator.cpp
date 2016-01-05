//
// Created by Jude Pereira on 05/01/2016.
//

#include "CMakeGenerator.h"

CMakeGenerator::CMakeGenerator(ofTargetPlatform platform, string projectPath, vector<string> &addons) {
    this->projectPath = projectPath;
    this->platform = platform;
    this->addons = addons;
}

void CMakeGenerator::generate() {
    ofFile cmakeListsFile(ofFilePath::join(projectPath, "CMakeLists.txt"), ofFile::Mode::ReadWrite, false);

    if (cmakeListsFile.exists()) {
        ofLogNotice() << "CMakeLists.txt already exists; update not supported yet";
        return;
    }

    cmakeListsFile.create();

    cmakeListsFile << "cmake_minimum_required(VERSION 2.8.4)\n"
            "cmake_policy(SET CMP0042 NEW)\n"
            "project(ofApplication)\n"
            "set(APP_NAME ofApplication)\n"
            "set(BUNDLE_NAME ofApplication)\n"
            "\n"
            "# ===================== oF Directory =====================\n"
            "set(OF_DIRECTORY \"" << getOFRoot() << "\" CACHE PATH \"openFrameworks path\")\n"
            "# ========================================================\n"
            "\n"
            "# ==================== Get oF Version ====================\n"
            "file(STRINGS ${OF_DIRECTORY}/libs/openFrameworks/utils/ofConstants.h OF_VERSION_MAJOR REGEX \"^(#define OF_VERSION_MAJOR)\")\n"
            "file(STRINGS ${OF_DIRECTORY}/libs/openFrameworks/utils/ofConstants.h OF_VERSION_MINOR REGEX \"^(#define OF_VERSION_MINOR)\")\n"
            "file(STRINGS ${OF_DIRECTORY}/libs/openFrameworks/utils/ofConstants.h OF_VERSION_PATCH REGEX \"^(#define OF_VERSION_PATCH)\")\n"
            "string(REPLACE \"#define OF_VERSION_MAJOR \" \"\" OF_VERSION_MAJOR ${OF_VERSION_MAJOR})\n"
            "string(REPLACE \"#define OF_VERSION_MINOR \" \"\" OF_VERSION_MINOR ${OF_VERSION_MINOR})\n"
            "string(REPLACE \"#define OF_VERSION_PATCH \" \"\" OF_VERSION_PATCH ${OF_VERSION_PATCH})\n"
            "set(OF_VERSION ${OF_VERSION_MAJOR}.${OF_VERSION_MINOR}.${OF_VERSION_PATCH})\n"
            "message(STATUS \"Detected openFrameworks version: ${OF_VERSION}\")\n"
            "# ========================================================\n"
            "\n"
            "# ===================== Compiler Flags =====================\n"
            "set(CMAKE_C_COMPILER \"/usr/bin/clang\")\n"
            "set(CMAKE_C_FLAGS \"\") # -x objective-c\n"
            "\n"
            "set(CMAKE_CXX_COMPILER \"/usr/bin/clang++\")\n"
            "if (${OF_VERSION} EQUAL \"0.9.0\")\n"
            "    set(CMAKE_CXX_FLAGS \"-std=c++11 -stdlib=libc++ -D__MACOSX_CORE__\") # Removed \"-stdlib=libstdc++\n"
            "elseif (${OF_VERSION} EQUAL \"0.8.4\")\n"
            "    set(CMAKE_CXX_FLAGS \"-stdlib=libstdc++ -D__MACOSX_CORE__\")\n"
            "endif ()\n"
            "\n"
            "set(CMAKE_OSX_ARCHITECTURES i386)\n"
            "add_compile_options(-Wno-deprecated-declarations)\n"
            "# ==========================================================\n"
            "\n"
            "# ===================== Source Files =====================\n"
            "set(SOURCE_FILES\n"
            "        src/main.cpp\n"
            "        src/ofApp.cpp)\n"
            "# ========================================================\n"
            "\n"
            "# === Include OpenFrameworks as an External Project ======\n"
            "set(of_install_dir ${PROJECT_SOURCE_DIR}/bin/${APP_NAME}.app/Contents/Frameworks)\n"
            "include(ExternalProject)\n"
            "ExternalProject_Add(\n"
            "        of_shared\n"
            "        SOURCE_DIR ${OF_DIRECTORY}\n"
            "        INSTALL_DIR ${of_install_dir}\n"
            "        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>\n"
            "        CMAKE_CACHE_ARGS -DINCLUDE_ADDONS:PATH=${PROJECT_SOURCE_DIR}/addons.cmake\n"
            ")\n"
            "# ================================================\n"
            "\n"
            "# ===================== System Frameworks =====================\n"
            "find_library(opengl_lib OpenGL)\n"
            "# =============================================================\n"
            "\n"
            "# ==================== Addons ====================\n";

    for (int i = 0; i < addons.size(); i++) {
        cmakeListsFile << "include(${OF_DIRECTORY}/addons/" << addons[i] << "/addon_config.cmake)\n";
    }

    cmakeListsFile << "# ================================================\n"
            "\n"
            "# ===================== Include Dirs =====================\n"
            "set(OF_CORE_HEADERS\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/3d\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/app\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/communication\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/events\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/gl\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/graphics\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/math\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/sound\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/types\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/utils\n"
            "        ${OF_DIRECTORY}/libs/openFrameworks/video\n"
            "        ${OF_DIRECTORY}/libs/freetype/include\n"
            "        ${OF_DIRECTORY}/libs/freetype/include/freetype2\n"
            "        ${OF_DIRECTORY}/libs/poco/include\n"
            "        ${OF_DIRECTORY}/libs/fmodex/include\n"
            "        ${OF_DIRECTORY}/libs/glew/include\n"
            "        ${OF_DIRECTORY}/libs/FreeImage/include\n"
            "        ${OF_DIRECTORY}/libs/tess2/include\n"
            "        ${OF_DIRECTORY}/libs/cairo/include/cairo\n"
            "        ${OF_DIRECTORY}/libs/rtAudio/include\n"
            "        ${OF_DIRECTORY}/libs/glfw/include\n"
            "        ${OF_DIRECTORY}/libs/openssl/include\n"
            "        ${OF_DIRECTORY}/libs/utf8cpp/include\n"
            "        ${OF_DIRECTORY}/libs/boost/include\n"
            "        )\n"
            "include_directories(${OF_CORE_HEADERS} ${OF_ADDON_HEADERS})\n"
            "# ========================================================\n"
            "\n"
            "# ============ Compile and Link ==================\n"
            "add_executable(${APP_NAME} MACOSX_BUNDLE ${SOURCE_FILES})\n"
            "add_dependencies(${APP_NAME} of_shared)\n"
            "target_link_libraries(${APP_NAME} ${opengl_lib} ${of_install_dir}/of/lib/libopenFrameworks.dylib)\n"
            "# ================================================\n"
            "\n"
            "ADD_CUSTOM_COMMAND(TARGET ${APP_NAME}\n"
            "        POST_BUILD\n"
            "        COMMAND ${CMAKE_INSTALL_NAME_TOOL}\n"
            "        ARGS -change \"@rpath/libopenFrameworks.dylib\" \"@loader_path/../Frameworks/oF/lib/libopenFrameworks.dylib\" $<TARGET_FILE:${APP_NAME}>\n"
            "        )\n"
            "\n"
            "set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_STRING \"Cmake openFrameworks CLion app\")\n"
            "set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING \"0.0.1 pre-release alpha\")\n"
            "set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${BUNDLE_NAME})\n"
            "set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING 0.0.1)\n"
            "set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION 0.0.1)\n"
            "set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_COPYRIGHT \"--YOUR NAME HERE--\")\n"
            "set_target_properties(${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)\n"
            "# ================================================\n"
            "\n"
            "# ==================== Clean =====================\n"
            "SET_DIRECTORY_PROPERTIES(PROPERTIES\n"
            "        ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_SOURCE_DIR}/bin/${APP_NAME}.app)\n"
            "# ================================================";

    cmakeListsFile.close();
}
