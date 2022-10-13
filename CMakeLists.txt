cmake_minimum_required(VERSION 3.2)

project("Design Patterns" VERSION 1.0)

#Factory method
add_executable(factory_method "test/factory_method.cpp")
set_target_properties(factory_method PROPERTIES OUTPUT_NAME "factory_method")
target_include_directories(factory_method PUBLIC "include")
set_property(TARGET factory_method PROPERTY CXX_STANDARD 23)

#undefined reference to `__imp_WSACleanup'
if(WIN32)
	target_link_libraries(factory_method wsock32 ws2_32)
endif()
