include_directories(.)

file(GLOB EXAMPLES_C "*.c")

foreach(EXAMPLE_FILE ${EXAMPLES_C})
	get_filename_component(EXAMPLE ${EXAMPLE_FILE} NAME_WE)
	set (SOURCE_FILES ${EXAMPLE_FILE})
	add_executable(${EXAMPLE} ${SOURCE_FILES} )
endforeach(EXAMPLE_FILE)

target_link_libraries(alsa-play asound)
