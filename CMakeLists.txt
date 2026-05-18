cmake_minimum_required(VERSION 3.20)
project(TSReader C CXX)

# ============================================================
# TSReader - Full Build using pre-built libs + source
# Build: cmake -G "Visual Studio 18 2026" -A Win32 -B build
#        cmake --build build --config Release
# ============================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

# ============================================================
# Options
# ============================================================
option(TSREADER_LITE "Build LITE version" OFF)
option(TSREADER_PRO "Build PRO version" OFF)

# ============================================================
# Global compile settings
# ============================================================
if(MSVC)
    add_compile_definitions(
        WIN32
        _WINDOWS
        NONOEM
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
        _WINSOCK_DEPRECATED_NO_WARNINGS
    )

    if(TSREADER_LITE)
        add_compile_definitions(LITE)
    endif()
    if(TSREADER_PRO)
        add_compile_definitions(PRO)
    endif()

    add_compile_options(
        /W3
        /wd4996 /wd4013 /wd4244 /wd4267 /wd4101 /wd4018
        /wd4047 /wd4024 /wd4133 /wd4028 /wd4005 /wd4090
        /wd4305 /wd4334 /wd5287
    )

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ============================================================
# TSReader_SourceHelper — built from source
# Ordinals 1-48 are real implementations from the in-repo C source;
# ordinals 49-63 are stubs (see TSReader_SourceHelper.c "Stubs for
# SourceHelper functions added between 2008 and 2018"). Building it
# ourselves means the VARIABLES struct layout always matches the host
# regardless of MAX_SOURCE_MODULES, fixing KNOWN_ISSUES BA-1.
# ============================================================
add_library(TSReader_SourceHelper SHARED
    TSReader_SourceHelper/TSReader_SourceHelper.c
    TSReader_SourceHelper/serial.c
    TSReader_SourceHelper/crc.c
    TSReader_SourceHelper/TSReader_SourceHelper.rc
    TSReader_SourceHelper/TSReader_SourceHelper.def
)
target_include_directories(TSReader_SourceHelper PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs
    ${CMAKE_CURRENT_SOURCE_DIR}/TSReader_SourceHelper
    ${CMAKE_CURRENT_SOURCE_DIR}/Md/MULTIDEC
)
target_link_libraries(TSReader_SourceHelper PRIVATE
    kernel32 user32 gdi32 advapi32 shell32
    ole32 oleaut32 comctl32 comdlg32 ws2_32 winmm
)

# ============================================================
# libfaad2 (pre-built DLL + import library)
# ============================================================
add_library(libfaad2_import SHARED IMPORTED)
set_target_properties(libfaad2_import PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/libfaad2.lib"
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libfaad2.dll"
)

# ============================================================
# libmad (MPEG audio decoder - build from source)
# ============================================================
add_library(libmad STATIC
    libmad-0.15.1b/bit.c
    libmad-0.15.1b/decoder.c
    libmad-0.15.1b/fixed.c
    libmad-0.15.1b/frame.c
    libmad-0.15.1b/huffman.c
    libmad-0.15.1b/layer12.c
    libmad-0.15.1b/layer3.c
    libmad-0.15.1b/stream.c
    libmad-0.15.1b/synth.c
    libmad-0.15.1b/timer.c
    libmad-0.15.1b/version.c
)
target_include_directories(libmad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libmad-0.15.1b
    ${CMAKE_CURRENT_SOURCE_DIR}/libmad-0.15.1b/msvc++
)
target_compile_definitions(libmad PRIVATE
    WIN32 _WINDOWS NDEBUG _CRT_SECURE_NO_WARNINGS
    HAVE_CONFIG_H FPM_INTEL ASO_ZEROCHECK
)
target_compile_options(libmad PRIVATE /wd4996 /wd4244 /wd4305)

# ============================================================
# liba52 + libao (AC3 decoder + audio output - build from source)
# ============================================================
add_library(liba52 STATIC
    a52dec-0.7.4/liba52/bit_allocate.c
    a52dec-0.7.4/liba52/bitstream.c
    a52dec-0.7.4/liba52/downmix.c
    a52dec-0.7.4/liba52/imdct.c
    a52dec-0.7.4/liba52/parse.c
)
target_include_directories(liba52 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/a52dec-0.7.4
    ${CMAKE_CURRENT_SOURCE_DIR}/a52dec-0.7.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/a52dec-0.7.4/liba52
)
target_compile_definitions(liba52 PRIVATE
    WIN32 _WINDOWS NDEBUG _CRT_SECURE_NO_WARNINGS
)
target_compile_options(liba52 PRIVATE /wd4996 /wd4244)

add_library(libao STATIC
    a52dec-0.7.4/libao/audio_out.c
    a52dec-0.7.4/libao/audio_out_aif.c
    a52dec-0.7.4/libao/audio_out_float.c
    a52dec-0.7.4/libao/audio_out_null.c
    a52dec-0.7.4/libao/audio_out_peak.c
    a52dec-0.7.4/libao/audio_out_wav.c
    a52dec-0.7.4/libao/audio_out_win.c
    a52dec-0.7.4/libao/float2s16.c
)
target_include_directories(libao PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/a52dec-0.7.4
    ${CMAKE_CURRENT_SOURCE_DIR}/a52dec-0.7.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/a52dec-0.7.4/libao
)
target_compile_definitions(libao PRIVATE
    WIN32 _WINDOWS NDEBUG _CRT_SECURE_NO_WARNINGS
    LIBAO_WIN
)
target_compile_options(libao PRIVATE /wd4996 /wd4244)

# ============================================================
# h264decoder (build from source)
# ============================================================
file(GLOB H264_SOURCES h264decoder/src/*.c)
add_library(h264decoder STATIC ${H264_SOURCES})
target_include_directories(h264decoder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/h264decoder/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/h264decoder/src
)
target_compile_definitions(h264decoder PRIVATE WIN32 _WINDOWS NDEBUG _CRT_SECURE_NO_WARNINGS)
target_compile_options(h264decoder PRIVATE /wd4996 /wd4244 /wd4334 /wd5287)

# ============================================================
# VC1 reference decoder (build from source)
# ============================================================
file(GLOB VC1_DEC_SRC VC1_reference_decoder/decoder/*.c)
file(GLOB VC1_SHARED_SRC VC1_reference_decoder/shared/*.c)
add_library(vc1decoder STATIC ${VC1_DEC_SRC} ${VC1_SHARED_SRC})
target_include_directories(vc1decoder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/VC1_reference_decoder/decoder
    ${CMAKE_CURRENT_SOURCE_DIR}/VC1_reference_decoder/shared
)
target_compile_definitions(vc1decoder PRIVATE WIN32 _WINDOWS NDEBUG _CRT_SECURE_NO_WARNINGS)
target_compile_options(vc1decoder PRIVATE /wd4996 /wd4244)

# ============================================================
# Pre-built libraries (from original VC6 build)
# ============================================================
add_library(libmpeg2_prebuilt STATIC IMPORTED)
set_target_properties(libmpeg2_prebuilt PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libmpeg2.lib")

add_library(libmpeg2convert_prebuilt STATIC IMPORTED)
set_target_properties(libmpeg2convert_prebuilt PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libmpeg2convert.lib")

add_library(libvo_prebuilt STATIC IMPORTED)
set_target_properties(libvo_prebuilt PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libvo.lib")

add_library(PEGRPCS STATIC IMPORTED)
set_target_properties(PEGRPCS PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/PEGRPCS.lib")

# ============================================================
# Main TSReader executable
# ============================================================
set(TSREADER_SOURCES
    TSReader.c
    parser.c
    formatter.c
    export.c
    settings.c
    util.c
    archive.c
    charting.c
    ControlServer.c
    EITServer.c
    email.c
    EPGGrid.c
    Forwarder.c
    GPSSignal.c
    mosaic.c
    ProfileBrowser.c
    reed-solomon.c
    RokuTelnetInterface.c
    sky_epg.c
    sky_huffman.c
    StreamMonitor.c
    TitleThumbnails.c
    ATSC_huffman.c
    bcdmux.c
    dn_huffman.c
    CCDecoder.c
    CI-CAM.c
    MDInterface.c
    MDI/mdi.c
    AACAudioDecoder.c
    AC3AudioDecoder.c
    MPEGAudio.c
    MPEG2Decoder.c
    H264Decoder.c
    MPEG4Decoder.c
    VC1Decoder.c
    SoftCSA.c
    stubs/link_stubs.c
    stubs/isource_impl.c
    splash.c
)

add_executable(TSReader WIN32
    ${TSREADER_SOURCES}
    TSReader.rc
)

target_include_directories(TSReader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs
    ${CMAKE_CURRENT_SOURCE_DIR}/libfaad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libfaad
    ${CMAKE_CURRENT_SOURCE_DIR}/h264decoder/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/VC1_reference_decoder/decoder
    ${CMAKE_CURRENT_SOURCE_DIR}/VC1_reference_decoder/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/Md/MULTIDEC
    ${CMAKE_CURRENT_SOURCE_DIR}/TSReader_Stradis
    ${CMAKE_CURRENT_SOURCE_DIR}/TSReader_SourceHelper
    ${CMAKE_CURRENT_SOURCE_DIR}/libmad-0.15.1b/msvc++
    ${CMAKE_CURRENT_SOURCE_DIR}/a52dec-0.7.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/a52dec-0.7.4/libao
)

set_target_properties(TSReader PROPERTIES OUTPUT_NAME "TSReaderPro")

# STANDARD_ICON gates the VS_VERSION_INFO block in TSReader.rc.
# Without it, rc.exe preprocesses the entire VERSIONINFO out and the
# resulting exe ships with FileVersion 0.0.0.0. CMake's MSVC generator
# propagates target_compile_definitions to rc.exe as well as cl.exe.
target_compile_definitions(TSReader PRIVATE STANDARD_ICON)

target_link_libraries(TSReader PRIVATE
    TSReader_SourceHelper
    h264decoder
    vc1decoder
    libfaad2_import
    libmad
    liba52
    libao
    libmpeg2_prebuilt
    libmpeg2convert_prebuilt
    libvo_prebuilt
    PEGRPCS
    kernel32 user32 gdi32 advapi32 shell32
    ole32 oleaut32 uuid setupapi comctl32
    winmm ws2_32 wininet comdlg32 winspool
    odbc32 odbccp32
)

# Copy runtime DLLs (TSReader_SourceHelper.dll is now built directly
# into $<TARGET_FILE_DIR:TSReader> by its own CMake target above)
foreach(DLL libfaad2.dll PEGRPCS.DLL)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DLL}")
        add_custom_command(TARGET TSReader POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${DLL}"
                "$<TARGET_FILE_DIR:TSReader>/${DLL}"
        )
    endif()
endforeach()

install(TARGETS TSReader RUNTIME DESTINATION bin)
