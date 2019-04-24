################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/exception/DatabaseCreateException.cpp \
../src/exception/DatabaseException.cpp \
../src/exception/Error.cpp \
../src/exception/FileNotFoundException.cpp \
../src/exception/IndexCreateException.cpp \
../src/exception/InsertDataException.cpp \
../src/exception/KeyNotFoundException.cpp \
../src/exception/QueryException.cpp \
../src/exception/SqlSyntaxException.cpp \
../src/exception/TableCreateException.cpp 

OBJS += \
./src/exception/DatabaseCreateException.o \
./src/exception/DatabaseException.o \
./src/exception/Error.o \
./src/exception/FileNotFoundException.o \
./src/exception/IndexCreateException.o \
./src/exception/InsertDataException.o \
./src/exception/KeyNotFoundException.o \
./src/exception/QueryException.o \
./src/exception/SqlSyntaxException.o \
./src/exception/TableCreateException.o 

CPP_DEPS += \
./src/exception/DatabaseCreateException.d \
./src/exception/DatabaseException.d \
./src/exception/Error.d \
./src/exception/FileNotFoundException.d \
./src/exception/IndexCreateException.d \
./src/exception/InsertDataException.d \
./src/exception/KeyNotFoundException.d \
./src/exception/QueryException.d \
./src/exception/SqlSyntaxException.d \
./src/exception/TableCreateException.d 


# Each subdirectory must supply rules for building sources it contributes
src/exception/%.o: ../src/exception/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


