################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/classification/AutoPredictor.cpp \
../src/classification/ConfusionMatrix.cpp \
../src/classification/CrossValidationError.cpp \
../src/classification/TimeseriesClassification.cpp 

OBJS += \
./src/classification/AutoPredictor.o \
./src/classification/ConfusionMatrix.o \
./src/classification/CrossValidationError.o \
./src/classification/TimeseriesClassification.o 

CPP_DEPS += \
./src/classification/AutoPredictor.d \
./src/classification/ConfusionMatrix.d \
./src/classification/CrossValidationError.d \
./src/classification/TimeseriesClassification.d 


# Each subdirectory must supply rules for building sources it contributes
src/classification/%.o: ../src/classification/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


