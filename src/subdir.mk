################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Block.cpp \
../src/BlockMgr.cpp \
../src/DBMS.cpp \
../src/Dictionary.cpp \
../src/Tuple.cpp \
../src/main.cpp 

OBJS += \
./src/Block.o \
./src/BlockMgr.o \
./src/DBMS.o \
./src/Dictionary.o \
./src/Tuple.o \
./src/main.o 

CPP_DEPS += \
./src/Block.d \
./src/BlockMgr.d \
./src/DBMS.d \
./src/Dictionary.d \
./src/Tuple.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


