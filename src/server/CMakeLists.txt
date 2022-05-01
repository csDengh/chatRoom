
aux_source_directory(. SRC_LISTS)

add_executable(chatServer ${SRC_LISTS})
target_link_libraries(chatServer muduo_net muduo_base mysqlclient hiredis pthread)