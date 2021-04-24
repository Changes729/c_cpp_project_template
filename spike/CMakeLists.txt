include_directories(.)
include_directories(include)
include_directories(shared)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

pkg_check_modules(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

pkg_check_modules(DBUS REQUIRED dbus-1)
include_directories(${DBUS_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})

pkg_check_modules(DBUS_GLIB REQUIRED dbus-glib-1)
include_directories(${DBUS_GLIB_INCLUDE_DIRS})
link_directories(${DBUS_GLIB_LIBRARY_DIRS})

pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
include_directories(${GOBJECT_INCLUDE_DIRS})
link_directories(${GOBJECT_LIBRARY_DIRS})

pkg_check_modules(GIO REQUIRED gio-2.0)
include_directories(${GIO_INCLUDE_DIRS})
link_directories(${GIO_LIBRARY_DIRS})

file(GLOB EXAMPLES_C "src/*.c")
file(GLOB SHARED_C "shared/*.c")

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

set(GLIB_DBUS_BINDING_FILES
	${CMAKE_CURRENT_BINARY_DIR}/client-bindings.h
	${CMAKE_CURRENT_BINARY_DIR}/server-bindings.h
)

foreach(EXAMPLE_FILE ${EXAMPLES_C})
	get_filename_component(EXAMPLE ${EXAMPLE_FILE} NAME_WE)
	set (SOURCE_FILES ${EXAMPLE_FILE})
	add_executable(${EXAMPLE} ${GLIB_DBUS_BINDING_FILES} ${SOURCE_FILES} ${SHARED_C})
endforeach(EXAMPLE_FILE)

target_link_libraries(alsa-play asound)

target_link_libraries(dbus-epoll ${DBUS_LIBRARIES})
target_link_libraries(dbus-server ${DBUS_LIBRARIES})
target_link_libraries(dbus-client ${DBUS_LIBRARIES})

target_link_libraries(glib-dbus-server ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${DBUS_GLIB_LIBRARIES})
target_link_libraries(glib-dbus-client ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${DBUS_GLIB_LIBRARIES})

target_link_libraries(glib-dbus-server2 ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${DBUS_GLIB_LIBRARIES})
target_link_libraries(glib-dbus-client2 ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${DBUS_GLIB_LIBRARIES} ${GIO_LIBRARIES})
