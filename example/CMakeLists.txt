include_directories(
        ${DELPHES_DIR}	
)

add_executable(delphesfcc-simpleWrite simplewriteWithDelphes.cc)

target_link_libraries(delphesfcc-simpleWrite utilities datamodel albers ${DELPHES_LIBRARIES})


install(TARGETS delphesfcc-simpleWrite DESTINATION bin)
