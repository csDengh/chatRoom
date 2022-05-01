
aux_source_directory(. SRC_LISTS_CLIENT)

add_executable(chatClient ${SRC_LISTS_CLIENT} )
target_link_libraries(chatClient pthread)