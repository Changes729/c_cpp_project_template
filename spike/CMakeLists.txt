# Include .h ###################################################################
include_directories(.)
include_directories(include)
include_directories(shared)
include_directories(shared/lib)
include_directories(shared/dbus)
include_directories(shared/dbus/initial)
include_directories(shared/dbus/interface)
include_directories(shared/dbus/object)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src/lvgl-lib)

## FetchContent 3rd-party lib ##################################################
include(FetchContent)

FetchContent_Declare(
  LVGL
  GIT_REPOSITORY git@github.com:lvgl/lvgl.git
  GIT_TAG v9.5.0
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(LV_BUILD_CONF_PATH ${CMAKE_CURRENT_LIST_DIR}/shared/lvgl/lv_conf.h)
FetchContent_MakeAvailable(LVGL)

# find library #################################################################
pkg_check_modules(GLIB glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

pkg_check_modules(DBUS dbus-1)
include_directories(${DBUS_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})

pkg_check_modules(DBUS_GLIB dbus-glib-1)
include_directories(${DBUS_GLIB_INCLUDE_DIRS})
link_directories(${DBUS_GLIB_LIBRARY_DIRS})

pkg_check_modules(GOBJECT gobject-2.0)
include_directories(${GOBJECT_INCLUDE_DIRS})
link_directories(${GOBJECT_LIBRARY_DIRS})

pkg_check_modules(GIO gio-2.0)
include_directories(${GIO_INCLUDE_DIRS})
link_directories(${GIO_LIBRARY_DIRS})

pkg_check_modules(GIOMM REQUIRED IMPORTED_TARGET "giomm-2.4")
include_directories(${GIOMM_INCLUDE_DIRS})
link_directories(${GIOMM_LIBRARY_DIRS})

find_package(SDL2)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})
if (NOT SDL2_FOUND)
    include(FetchContent)
    if (MSVC)
        FetchContent_Declare(
                SDL2
                URL https://www.libsdl.org/release/SDL2-devel-2.0.9-VC.zip
                URL_MD5 22bdb85ce4874673c4fafcce772d8b72
        )
    elseif (MINGW)
        FetchContent_Declare(
                SDL2
                URL https://www.libsdl.org/release/SDL2-devel-2.0.9-mingw.tar.gz
                URL_MD5 f5645eed64214c3bc22a3e157fc1f15f
        )
    else ()
        message(FATAL_ERROR "SDL2 not available")
    endif ()
    FetchContent_MakeAvailable(SDL2)
    set(SDL2_ROOT_DIR ${sdl2_SOURCE_DIR})
endif ()

pkg_check_modules(LIBUSB libusb-1.0)
include_directories(${LIBUSB_INCLUDE_DIRS})
link_directories(${LIBUSB_LIBRARY_DIRS})


# include .c/.cpp ##############################################################
file(GLOB EXAMPLES_C "src/*.c" "src/*.cpp")
file(GLOB DESIGN_PATTERN_C "src/design-pattern/*.c" "src/design-pattern/*.cpp")
file(GLOB SHARED_C "shared/*.c")
file(GLOB LIB_SRC "shared/lib/*.c" "shared/lib/*.cpp")

file(GLOB LIB_DBUS_HELPER_SRC
		"shared/dbus/*.c"
		"shared/dbus/initial/*.c"
		"shared/dbus/interface/*.c"
		"shared/dbus/interface/introspectable/*.c"
		"shared/dbus/interface/object-manager/*.c"
		"shared/dbus/interface/properties/*.c"
		"shared/dbus/object/*.c"
)

file(GLOB EXAMPLE_DBUS "src/dbus-object-example/object/*.c")

file(GLOB LVGL_LIB_SRC
		"src/lvgl-lib/*.c"
		"src/lvgl-lib/display_backends/*.c"
		"src/lvgl-lib/indev_backends/*.c"
)

# generate .h ##################################################################
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/server-bindings.h
  COMMAND dbus-binding-tool
	ARGS --prefix=server_object --mode=glib-server ${CMAKE_SOURCE_DIR}/spike/servicename-infos.xml --output=${CMAKE_CURRENT_BINARY_DIR}/server-bindings.h
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/client-bindings.h
	COMMAND dbus-binding-tool
	ARGS --prefix=server_object --mode=glib-client ${CMAKE_SOURCE_DIR}/spike/servicename-infos.xml --output=${CMAKE_CURRENT_BINARY_DIR}/client-bindings.h
	VERBATIM
)

## dbus-used include .h ##############################################
set(GLIB_DBUS_BINDING_FILES
	${CMAKE_CURRENT_BINARY_DIR}/client-bindings.h
	${CMAKE_CURRENT_BINARY_DIR}/server-bindings.h
)

# add library ##################################################################
add_library(project_library ${LIB_SRC})

add_library(lib_dbus_helper ${LIB_DBUS_HELPER_SRC})
target_link_libraries(lib_dbus_helper project_library ${DBUS_LIBRARIES})

add_library(dbus_example_object ${EXAMPLE_DBUS} ${LIB_DBUS_HELPER_SRC})
target_link_libraries(dbus_example_object project_library ${DBUS_LIBRARIES})

add_library(lvgl_lib ${LVGL_LIB_SRC})
target_link_libraries(lvgl_lib lvgl)


# generate exec obj ############################################################
foreach(EXAMPLE_FILE ${EXAMPLES_C})
	get_filename_component(EXAMPLE ${EXAMPLE_FILE} NAME_WE)
	set (SOURCE_FILES ${EXAMPLE_FILE})
	add_executable(${EXAMPLE} ${GLIB_DBUS_BINDING_FILES} ${SOURCE_FILES} ${SHARED_C})
	target_link_libraries(${EXAMPLE} project_library)
endforeach(EXAMPLE_FILE)

foreach(EXAMPLE_FILE ${DESIGN_PATTERN_C})
	get_filename_component(EXAMPLE ${EXAMPLE_FILE} NAME_WE)
	set (SOURCE_FILES ${EXAMPLE_FILE})
	add_executable(${EXAMPLE} ${GLIB_DBUS_BINDING_FILES} ${SOURCE_FILES} ${SHARED_C})
	target_link_libraries(${EXAMPLE} project_library)
endforeach(EXAMPLE_FILE)

# link librarys ################################################################
target_link_libraries(alsa-play asound)
target_link_libraries(io-flush-main project_library)
target_link_libraries(example-timer project_library)

target_link_libraries(dbus-poll ${DBUS_LIBRARIES})
target_link_libraries(dbus-server ${DBUS_LIBRARIES} dbus_example_object)
target_link_libraries(dbus-client ${DBUS_LIBRARIES})

target_link_libraries(glib-dbus-server ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${DBUS_GLIB_LIBRARIES})
target_link_libraries(glib-dbus-client ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${DBUS_GLIB_LIBRARIES})

target_link_libraries(glib-dbus-server2 ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${DBUS_GLIB_LIBRARIES})
target_link_libraries(glib-dbus-client2 ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${DBUS_GLIB_LIBRARIES} ${GIO_LIBRARIES})

target_link_libraries(thread_mem_attack pthread)
target_link_libraries(sdl_triangle ${SDL2_LIBRARIES})

target_link_libraries(readline-example readline)
target_link_libraries(testlibusb ${LIBUSB_LIBRARIES})
target_link_libraries(udev_device udev)
target_link_libraries(mime-type PkgConfig::GIOMM)

target_link_libraries(mime-type-macos
  PkgConfig::GIOMM
  "-framework CoreFoundation"
  "-framework ApplicationServices"
)


target_link_libraries(lvgl-simple lvgl lvgl_lib)
target_link_libraries(lvgl PRIVATE SDL2::SDL2)
