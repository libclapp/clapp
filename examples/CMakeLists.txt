if(libClaPP_FS_LINKER_FLAG)
    add_executable(libclapp_example_full full_example.cpp)
    target_link_libraries(libclapp_example_full clapp ${libClaPP_FS_LINKER_FLAG})
    install(TARGETS libclapp_example_full DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

add_executable(libclapp_example_short short_example.cpp)
target_link_libraries(libclapp_example_short clapp)
install(TARGETS libclapp_example_short DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(libclapp_example_sub_parser sub_parser_example.cpp)
target_link_libraries(libclapp_example_sub_parser clapp)
install(TARGETS libclapp_example_sub_parser DESTINATION ${CMAKE_INSTALL_BINDIR})
