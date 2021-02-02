set(COMPONENT_SRCS 
		"src/jbdrivers/JbController.cpp"
		"src/jbdrivers/UartChannel.cpp"
		"src/jbdrivers/SpiMaster.cpp"
		"src/jbdrivers/Sx127xSpiDevice.cpp"
		"src/jbdrivers/GpioInterrupt.cpp"
		"src/jbdrivers/VoidTimer.cpp"
		"src/jbdrivers/Encoder.cpp")
set(COMPONENT_ADD_INCLUDEDIRS 
		"include")
		
set(COMPONENT_REQUIRES jblib-platform-abstract-jbkernel)

register_component()