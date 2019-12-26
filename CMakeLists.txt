set(COMPONENT_SRCS 
		"src/jbdrivers/JbController.cpp")
set(COMPONENT_ADD_INCLUDEDIRS 
		"include")
		
set(COMPONENT_REQUIRES jblib-platform-abstract-jbkernel)

register_component()