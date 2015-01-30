set(searchpath ${DELPHES_DIR} $ENV{DELPHES} ${DELPHES_DIR}/install ${DELPHES_DIR}/external )


find_library(DELPHES_LIBRARY
              NAMES Delphes delphes
              HINTS ${searchpath}
              PATH_SUFFIXES lib)

find_path(DELPHES_INCLUDE_DIR
        #   NAMES DelphesClasses.h Delphes.h
           NAMES classes/DelphesClasses.h modules/Delphes.h ExRootAnalysis/ExRootTask.h
           HINTS ${searchpath}
           PATH_SUFFIXES include)


unset(searchpath)

set(DELPHES_INCLUDE_DIRS ${DELPHES_INCLUDE_DIR})
set(DELPHES_LIBRARIES ${DELPHES_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set DELPHES_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Delphes DEFAULT_MSG DELPHES_INCLUDE_DIR DELPHES_LIBRARY)

mark_as_advanced(DELPHES_INCLUDE_DIR DELPHES_LIBRARY)