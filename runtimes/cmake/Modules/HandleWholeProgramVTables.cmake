if(LLVM_ENABLE_LTO)
  # Whole-program vtables enable split LTO units; C and C++ must agree at link time.
  if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND
     NOT (CMAKE_LINKER_TYPE STREQUAL "LLD" OR LINKER_IS_LLD))
    # Apple's legacy ThinLTO interface cannot consume split LTO units.
    append("-fno-split-lto-unit" CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
  elseif(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    append("/clang:-fsplit-lto-unit" CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
  else()
    append("-fsplit-lto-unit" CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
  endif()
endif()

function(runtimes_enable_whole_program_vtables directory)
  set(vtable_option "-fwhole-program-vtables")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(vtable_option "/clang:-fwhole-program-vtables")
  endif()
  get_directory_property(targets DIRECTORY "${directory}" BUILDSYSTEM_TARGETS)
  get_directory_property(cxx_flags DIRECTORY "${directory}"
    DEFINITION CMAKE_CXX_FLAGS)
  string(TOUPPER "${CMAKE_BUILD_TYPE}" configuration)
  get_directory_property(config_flags DIRECTORY "${directory}"
    DEFINITION "CMAKE_CXX_FLAGS_${configuration}")

  foreach(target IN LISTS targets)
    get_target_property(type "${target}" TYPE)
    if(NOT type MATCHES "^(OBJECT_LIBRARY|STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|EXECUTABLE)$")
      continue()
    endif()
    get_target_property(target_flags "${target}" COMPILE_FLAGS)
    get_target_property(target_options "${target}" COMPILE_OPTIONS)
    get_target_property(source_directory "${target}" SOURCE_DIR)
    get_target_property(sources "${target}" SOURCES)
    string(CONCAT target_compile_flags
      "${cxx_flags} ${config_flags} ${target_flags} ${target_options}")
    string(REGEX MATCHALL "(^|[ ;])(/clang:)?-f(no-)?lto(=[^ ;]+)?"
      target_lto_options "${target_compile_flags}")
    set(target_lto OFF)
    if(target_lto_options)
      list(GET target_lto_options -1 target_lto_option)
      if(NOT target_lto_option MATCHES "-fno-lto")
        set(target_lto ON)
      endif()
    endif()
    set(lto_sources)
    set(non_lto_sources)
    foreach(source IN LISTS sources)
      if(source MATCHES "\\$<")
        continue()
      endif()
      get_filename_component(source "${source}" ABSOLUTE
        BASE_DIR "${source_directory}")
      get_source_file_property(language "${source}"
        TARGET_DIRECTORY "${target}" LANGUAGE)
      if(NOT language STREQUAL "CXX" AND NOT source MATCHES "\\.(cpp|cc|cxx|C)$")
        continue()
      endif()
      get_source_file_property(source_flags "${source}"
        TARGET_DIRECTORY "${target}" COMPILE_FLAGS)
      get_source_file_property(source_options "${source}"
        TARGET_DIRECTORY "${target}" COMPILE_OPTIONS)

      # Clang's last LTO option wins, including compiler-rt's explicit opt-outs.
      string(CONCAT flags
        "${cxx_flags} ${config_flags} ${target_flags} ${target_options} "
        "${source_flags} ${source_options}")
      string(REGEX MATCHALL "(^|[ ;])(/clang:)?-f(no-)?lto(=[^ ;]+)?" lto_options "${flags}")
      if(NOT lto_options)
        list(APPEND non_lto_sources "${source}")
        continue()
      endif()
      list(GET lto_options -1 lto_option)
      if(lto_option MATCHES "-fno-lto")
        list(APPEND non_lto_sources "${source}")
        continue()
      endif()
      list(APPEND lto_sources "${source}")
    endforeach()

    if(target_lto AND lto_sources AND NOT non_lto_sources)
      # Target options also cover CMake-generated C++ sources such as PCHs.
      target_compile_options("${target}" PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${vtable_option}>")
    else()
      foreach(source IN LISTS lto_sources)
        # A source can belong to both LTO and non-LTO targets in one directory.
        set_property(SOURCE "${source}" TARGET_DIRECTORY "${target}"
          APPEND PROPERTY COMPILE_OPTIONS
          "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<STREQUAL:$<TARGET_PROPERTY:NAME>,${target}>>:${vtable_option}>")
      endforeach()
    endif()
  endforeach()

  get_directory_property(subdirectories DIRECTORY "${directory}" SUBDIRECTORIES)
  foreach(subdirectory IN LISTS subdirectories)
    runtimes_enable_whole_program_vtables("${subdirectory}")
  endforeach()
endfunction()
