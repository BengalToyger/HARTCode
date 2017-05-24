################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../AT24CMO2.c \
../Firing.c \
../Kalman.c \
../main.c \
../MLX90393.c \
../MPU6000.c \
../MS5607.c \
../PAM7Q.c \
../radio_ctl.c \
../Rocket.c \
../RocketTiming.c \
../SPIComm.c \
../TWIComm.c \
../unittest.c \
../USARTComm.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
AT24CMO2.o \
Firing.o \
Kalman.o \
main.o \
MLX90393.o \
MPU6000.o \
MS5607.o \
PAM7Q.o \
radio_ctl.o \
Rocket.o \
RocketTiming.o \
SPIComm.o \
TWIComm.o \
unittest.o \
USARTComm.o

OBJS_AS_ARGS +=  \
AT24CMO2.o \
Firing.o \
Kalman.o \
main.o \
MLX90393.o \
MPU6000.o \
MS5607.o \
PAM7Q.o \
radio_ctl.o \
Rocket.o \
RocketTiming.o \
SPIComm.o \
TWIComm.o \
unittest.o \
USARTComm.o

C_DEPS +=  \
AT24CMO2.d \
Firing.d \
Kalman.d \
main.d \
MLX90393.d \
MPU6000.d \
MS5607.d \
PAM7Q.d \
radio_ctl.d \
Rocket.d \
RocketTiming.d \
SPIComm.d \
TWIComm.d \
unittest.d \
USARTComm.d

C_DEPS_AS_ARGS +=  \
AT24CMO2.d \
Firing.d \
Kalman.d \
main.d \
MLX90393.d \
MPU6000.d \
MS5607.d \
PAM7Q.d \
radio_ctl.d \
Rocket.d \
RocketTiming.d \
SPIComm.d \
TWIComm.d \
unittest.d \
USARTComm.d

OUTPUT_FILE_PATH +=HARTCode.elf

OUTPUT_FILE_PATH_AS_ARGS +=HARTCode.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler































./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.1.130\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g2 -Wall -Wextra -pedantic -mmcu=atmega2561 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.1.130\gcc\dev\atmega2561" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="HARTCode.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=atmega2561 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.1.130\gcc\dev\atmega2561"  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "HARTCode.elf" "HARTCode.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "HARTCode.elf" "HARTCode.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "HARTCode.elf" > "HARTCode.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "HARTCode.elf" "HARTCode.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "HARTCode.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "HARTCode.elf" "HARTCode.a" "HARTCode.hex" "HARTCode.lss" "HARTCode.eep" "HARTCode.map" "HARTCode.srec" "HARTCode.usersignatures"
	