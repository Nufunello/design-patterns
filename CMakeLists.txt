cmake_minimum_required(VERSION 3.2)

project("Design Patterns" VERSION 1.0)

set(design_patterns "factory_method" "abstract_factory" "builder" "prototype")

foreach(design_pattern IN LISTS design_patterns)
	add_executable("${design_pattern}" "test/${design_pattern}.cpp")
	set_target_properties("${design_pattern}" PROPERTIES OUTPUT_NAME "${design_pattern}")
	target_include_directories("${design_pattern}" PUBLIC "include")
	set_property(TARGET "${design_pattern}" PROPERTY CXX_STANDARD 23)
endforeach()
