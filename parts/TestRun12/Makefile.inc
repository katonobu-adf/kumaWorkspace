mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_COBJS += balancer.o balancer_param.o

APPL_CXXOBJS += \
	BalancingWalker.o \
	LineMonitor.o \
	BalancerCpp.o \
	Navigator.o \
	Driver.o \
	Task.o \
	ReadyToStart.o \
	Calibration.o \
	StartToDash.o \
	LineTracer.o \
	EmergencyStop.o \
	PID.o \
	Logging.o \
	Ev3Logger.o

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-ev3/Makefile

endif

APPL_DIR += $(mkfile_path)app $(mkfile_path)unit

INCLUDES += \
	-I$(mkfile_path)app \
	-I$(mkfile_path)unit
