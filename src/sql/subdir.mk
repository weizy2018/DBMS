################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sql/CreateSql.cpp \
../src/sql/DescSql.cpp \
../src/sql/DropSql.cpp \
../src/sql/ExecuteStatus.cpp \
../src/sql/InsertSql.cpp \
../src/sql/SQL.cpp \
../src/sql/SelectSql.cpp \
../src/sql/ShowSql.cpp \
../src/sql/UpdateSql.cpp \
../src/sql/UseSql.cpp 

OBJS += \
./src/sql/CreateSql.o \
./src/sql/DescSql.o \
./src/sql/DropSql.o \
./src/sql/ExecuteStatus.o \
./src/sql/InsertSql.o \
./src/sql/SQL.o \
./src/sql/SelectSql.o \
./src/sql/ShowSql.o \
./src/sql/UpdateSql.o \
./src/sql/UseSql.o 

CPP_DEPS += \
./src/sql/CreateSql.d \
./src/sql/DescSql.d \
./src/sql/DropSql.d \
./src/sql/ExecuteStatus.d \
./src/sql/InsertSql.d \
./src/sql/SQL.d \
./src/sql/SelectSql.d \
./src/sql/ShowSql.d \
./src/sql/UpdateSql.d \
./src/sql/UseSql.d 


# Each subdirectory must supply rules for building sources it contributes
src/sql/%.o: ../src/sql/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


