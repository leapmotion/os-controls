if(WIN32)

  set(_shortcuts "${CMAKE_INSTALL_PREFIX}/Shortcuts.exe")

  execute_process(
    COMMAND mt.exe "-updateresource:${_shortcuts}" -manifest uiAccess.manifest
    OUTPUT_FILE manifestlogout.txt ERROR_FILE manifestlogerror.txt RESULT_VARIABLE _runresult
  )
  execute_process(
    COMMAND signtool.exe sign -debug -v -a -n "Leap Motion Inc" 
      -t http://timestamp.verisign.com/scripts/timstamp.dll ${_shortcuts}
    OUTPUT_FILE signlogout.txt ERROR_FILE signlogerror.txt RESULT_VARIABLE _runresult
  )

if(NOT _runresult EQUAL 0)
  message(WARNING "Signing failed - see logout.txt and logerror.txt for more info.")
endif()

endif()