if(WIN32)

  set(_shortcuts "${CMAKE_INSTALL_PREFIX}/Shortcuts.exe")
  message(working_dir=${CMAKE_CURRENT_LIST_DIR})

  find_program(ManifestTool mt.exe PATHS "C:\\Program Files (x86)\\Windows Kits")
  execute_process(
    COMMAND mt.exe "-updateresource:${_shortcuts}" -manifest uiAccess.manifest
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_FILE logout.txt ERROR_FILE logerror.txt
  )

endif()