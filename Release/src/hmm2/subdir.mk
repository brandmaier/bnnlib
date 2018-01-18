################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/hmm2/HMM.cpp \
../src/hmm2/NormalDistribution.cpp 

OBJS += \
./src/hmm2/HMM.o \
./src/hmm2/NormalDistribution.o 

CPP_DEPS += \
./src/hmm2/HMM.d \
./src/hmm2/NormalDistribution.d 


# Each subdirectory must supply rules for building sources it contributes
src/hmm2/%.o: ../src/hmm2/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/eigen2 -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


