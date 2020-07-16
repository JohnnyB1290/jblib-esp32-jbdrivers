set(COMPONENT_SRCS 
		"src/jbdrivers/JbController.cpp"
		"src/jbdrivers/UartChannel.cpp")
set(COMPONENT_ADD_INCLUDEDIRS 
		"include")
		
set(COMPONENT_REQUIRES jblib-platform-abstract-jbkernel)

register_component()