################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Connection.cpp \
../src/DataSet.cpp \
../src/GnuplotGenerator.cpp \
../src/LSTMCascade.cpp \
../src/Network.cpp \
../src/NetworkFactory.cpp \
../src/Sequence.cpp \
../src/SequenceSet.cpp \
../src/TrainingConfiguration.cpp \
../src/functions.cpp \
../src/main.cpp 

OBJS += \
./src/Connection.o \
./src/DataSet.o \
./src/GnuplotGenerator.o \
./src/LSTMCascade.o \
./src/Network.o \
./src/NetworkFactory.o \
./src/Sequence.o \
./src/SequenceSet.o \
./src/TrainingConfiguration.o \
./src/functions.o \
./src/main.o 

CPP_DEPS += \
./src/Connection.d \
./src/DataSet.d \
./src/GnuplotGenerator.d \
./src/LSTMCascade.d \
./src/Network.d \
./src/NetworkFactory.d \
./src/Sequence.d \
./src/SequenceSet.d \
./src/TrainingConfiguration.d \
./src/functions.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


