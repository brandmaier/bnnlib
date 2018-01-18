################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ensembles/ConditionalBiasEnsemble.cpp \
../src/ensembles/DelayEnsemble.cpp \
../src/ensembles/Ensemble.cpp \
../src/ensembles/FeedforwardEnsemble.cpp \
../src/ensembles/GateEnsemble.cpp \
../src/ensembles/HarmonicGeneratorEnsemble.cpp \
../src/ensembles/KeepEnsemble.cpp \
../src/ensembles/LSTMCopyRestore.cpp \
../src/ensembles/LSTMCounterEnsemble.cpp \
../src/ensembles/LSTMEnsemble.cpp \
../src/ensembles/LSTMForgetEnsemble.cpp \
../src/ensembles/LSTMPlusEnsemble.cpp \
../src/ensembles/RecurrentEnsemble.cpp \
../src/ensembles/SecondOrderEnsemble.cpp \
../src/ensembles/WinnerTakesAllEnsemble.cpp 

OBJS += \
./src/ensembles/ConditionalBiasEnsemble.o \
./src/ensembles/DelayEnsemble.o \
./src/ensembles/Ensemble.o \
./src/ensembles/FeedforwardEnsemble.o \
./src/ensembles/GateEnsemble.o \
./src/ensembles/HarmonicGeneratorEnsemble.o \
./src/ensembles/KeepEnsemble.o \
./src/ensembles/LSTMCopyRestore.o \
./src/ensembles/LSTMCounterEnsemble.o \
./src/ensembles/LSTMEnsemble.o \
./src/ensembles/LSTMForgetEnsemble.o \
./src/ensembles/LSTMPlusEnsemble.o \
./src/ensembles/RecurrentEnsemble.o \
./src/ensembles/SecondOrderEnsemble.o \
./src/ensembles/WinnerTakesAllEnsemble.o 

CPP_DEPS += \
./src/ensembles/ConditionalBiasEnsemble.d \
./src/ensembles/DelayEnsemble.d \
./src/ensembles/Ensemble.d \
./src/ensembles/FeedforwardEnsemble.d \
./src/ensembles/GateEnsemble.d \
./src/ensembles/HarmonicGeneratorEnsemble.d \
./src/ensembles/KeepEnsemble.d \
./src/ensembles/LSTMCopyRestore.d \
./src/ensembles/LSTMCounterEnsemble.d \
./src/ensembles/LSTMEnsemble.d \
./src/ensembles/LSTMForgetEnsemble.d \
./src/ensembles/LSTMPlusEnsemble.d \
./src/ensembles/RecurrentEnsemble.d \
./src/ensembles/SecondOrderEnsemble.d \
./src/ensembles/WinnerTakesAllEnsemble.d 


# Each subdirectory must supply rules for building sources it contributes
src/ensembles/%.o: ../src/ensembles/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


