if(WIN32)

  set(_shortcuts "${CMAKE_INSTALL_PREFIX}/Shortcuts.exe")
  message(working_dir=${CMAKE_CURRENT_LIST_DIR})

  find_program(ManifestTool mt.exe PATHS "C:\\Program Files (x86)\\Windows Kits")
  execute_process(
    COMMAND mt.exe "-updateresource:${_shortcuts}" -manifest uiAccess.manifest
    COMMAND signtool.exe sign -debug -v -a -n "Leap Motion Inc" 
      -t http://timestamp.verisign.com/scripts/timstamp.dll ${_shortcuts}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_FILE logout.txt ERROR_FILE logerror.txt RESULT_VARIABLE _runresult
  )

if(NOT _runresult EQUAL 0)
  message(WARNING "Signing failed - see logout.txt and logerror.txt for more info.")
endif()

endif()