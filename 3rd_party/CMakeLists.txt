#3rd_party
set(3RD_PARTY_DIR ${CMAKE_CURRENT_LIST_DIR})

include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/dddb219c3eb96d7f9200f09b0a381f016e6b4562.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.92.5-docking # last stable version
  GIT_PROGRESS TRUE
  SOURCE_DIR imgui
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  imgui_filebrowser
  GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser.git
  GIT_TAG        dockable
  GIT_PROGRESS TRUE
  SOURCE_DIR imgui_filebrowser
)
FetchContent_MakeAvailable(imgui_filebrowser)
add_subdirectory(imgui)

if(WIN32)
  set(PUGIXML_WCHAR_MODE YES)
endif()

FetchContent_Declare(
  pugixml
  GIT_REPOSITORY https://github.com/zeux/pugixml.git
  GIT_TAG        v1.15 # last stable version
  GIT_PROGRESS TRUE
  SOURCE_DIR pugixml
)
FetchContent_MakeAvailable(pugixml)

FetchContent_Declare(ryml
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    GIT_TAG v0.9.0
    GIT_PROGRESS TRUE
    GIT_SHALLOW FALSE  # ensure submodules are checked out
    SOURCE_DIR ryml
)
FetchContent_MakeAvailable(ryml)

FetchContent_Declare(quill
    GIT_REPOSITORY https://github.com/odygrd/quill.git
    GIT_TAG v11.0.2
    GIT_PROGRESS TRUE
    SOURCE_DIR quill
)
FetchContent_MakeAvailable(quill)
add_subdirectory(quill)


set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.4
    GIT_PROGRESS TRUE
    SOURCE_DIR benchmark
)
FetchContent_MakeAvailable(benchmark)
set(BASE64_ENABLE_TESTING OFF)
FetchContent_Declare(base64
    GIT_REPOSITORY https://github.com/tobiaslocker/base64.git
    GIT_TAG 0d0f5a8849a954d96af7befdb5c4014e78ecc9e2
    GIT_PROGRESS TRUE
    SOURCE_DIR base64
)
FetchContent_MakeAvailable(base64)

FetchContent_Declare(concurrentqueue
    GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
    GIT_TAG v1.0.4
    GIT_PROGRESS TRUE
    SOURCE_DIR concurrentqueue
)
FetchContent_MakeAvailable(concurrentqueue)