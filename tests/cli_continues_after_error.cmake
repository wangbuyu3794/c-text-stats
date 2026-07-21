if(NOT DEFINED CLI OR NOT DEFINED MISSING_FILE OR NOT DEFINED VALID_FILE)
    message(FATAL_ERROR "CLI, MISSING_FILE, and VALID_FILE are required")
endif()

execute_process(
    COMMAND "${CLI}" "${MISSING_FILE}" "${VALID_FILE}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

if(NOT "${result}" STREQUAL "1")
    message(FATAL_ERROR "Expected exit code 1, got: ${result}")
endif()

if(NOT "${output}" MATCHES "LICENSE")
    message(FATAL_ERROR "Valid input was not processed: ${output}")
endif()

if(NOT "${output}" MATCHES "total")
    message(FATAL_ERROR "Total was not printed: ${output}")
endif()

if(NOT "${error}" MATCHES "cannot open")
    message(FATAL_ERROR "Missing-file error was not reported: ${error}")
endif()
