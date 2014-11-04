set(_this_is_standalone_project FALSE)
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}/source)
    set(_this_is_standalone_project TRUE)
endif()

if(_this_is_standalone_project)
	# Root dependency
	find_package(Components REQUIRED)

	# # This directory is a basis for all other includes
	# include_directories(.)
endif()

# All libraries for this app
add_subdirectory(VRIntro)
add_subdirectory(VRIntroLib)
