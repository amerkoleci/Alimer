if (NOT ALIMER_BUILD_SAMPLES)
	return()
endif ()

function(add_sample sample_name)
	file(GLOB SOURCE_FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/${sample_name}/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/${sample_name}/*.cpp"
	)

	if (ANDROID)
		add_library(${sample_name} SHARED ${SOURCE_FILES})
	else ()
		add_executable(${sample_name} ${SOURCE_FILES})
	endif ()
	target_link_libraries(${sample_name} Alimer)

	if (MSVC)
		set_property(TARGET ${sample_name} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${ALIMER_ROOT_DIR}")
	endif ()
	
	if (WINDOWS_STORE)
		set_target_properties(${sample_name} PROPERTIES VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION "${ALIMER_UWP_VERSION_MIN}")
	endif ()

	install(
		TARGETS ${sample_name}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
	)

	set_property(TARGET ${sample_name} PROPERTY FOLDER "Samples")
endfunction()

add_sample(DrawTriangle)
