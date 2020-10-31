set(COMPONENT_SRCS 
		"src/jbdrivers/JbController.cpp"
		"src/jbdrivers/UartChannel.cpp"
		"src/jbdrivers/SpiMaster.cpp"
		"src/jbdrivers/Sx127xSpiDevice.cpp")
set(COMPONENT_ADD_INCLUDEDIRS 
		"include")
		
set(COMPONENT_REQUIRES jblib-platform-abstract-jbkernel)

register_component()