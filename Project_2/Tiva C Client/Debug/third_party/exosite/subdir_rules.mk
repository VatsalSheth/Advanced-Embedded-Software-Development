################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
third_party/exosite/exosite.obj: C:/ti/TivaWare_C_Series-2.1.4.178/third_party/exosite/exosite.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/include" --include_path="C:/Users/vkshe/workspace_v8/Advanced-Embedded-Software-Development/Project_2/Tiva C Client" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/exosite" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power="all" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="third_party/exosite/exosite.d_raw" --obj_directory="third_party/exosite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

third_party/exosite/exosite_meta.obj: C:/ti/TivaWare_C_Series-2.1.4.178/third_party/exosite/exosite_meta.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/include" --include_path="C:/Users/vkshe/workspace_v8/Advanced-Embedded-Software-Development/Project_2/Tiva C Client" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/examples/boards/ek-tm4c1294xl-boostxl-senshub" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/exosite" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/lwip-1.4.1/src/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/lwip-1.4.1/src/include/ipv4" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/lwip-1.4.1/apps" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/lwip-1.4.1/ports/tiva-tm4c129/include" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power="all" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=TARGET_IS_TM4C129_RA0 --define=UART_BUFFERED --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="third_party/exosite/exosite_meta.d_raw" --obj_directory="third_party/exosite" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


