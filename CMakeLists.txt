
# 1.工程名

set(PROJECT_NAME Generic-Bare-Metal-MCU-Framework)

# 2.优化等级
#   [-O0]：不优化 
#   [-Ofast]：运行速度最快（Release）
#   [-Os]：代码尺寸最小 
#   [-Og]：有调试信息（Debug）

set(OPTIMIZATION -O0)

# 3.二进制文件名

set(SREC_NAME ${PROJECT_NAME})

# 4.链接脚本路径

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Frame/01_mcu/stm32f4_flash.ld)

# 5.全局宏定义（宏定义前面加-D）

add_definitions(-DSTM32F42_43xxx -DUSE_STDPERIPH_DRIVER -DUSE_USB_OTG_HS -DUSE_EMBEDDED_PHY )

# 6.头文件包含路径（尽量使用相对路径）

include_directories(

        main
        APP
        Frame
        Frame/01_mcu
        Frame/01_mcu/Libraries/CMSIS/Include
        Frame/01_mcu/Libraries/Device/ST/STM32F4xx/Include
        Frame/01_mcu/Libraries/STM32F4xx_StdPeriph_Driver/inc

        Frame/02_hal
        Frame/03_chip_driver
        Frame/04_utils_cpp
        Frame/05_device_cpp
)

#编译环境版本设置
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.19)

# 编译工具
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#工程名
project(${PROJECT_NAME} C CXX ASM)

#c/c++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# for use printf & scanf with float
#set(COMMON_FLAGS "-specs=nosys.specs -specs=nano.specs -u _printf_float -u _scanf_float")

#编译指令
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_compile_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

#源文件包含
file(GLOB_RECURSE SOURCES
        "main/*.cpp"
        "APP/*.cpp"
        "Frame/01_mcu/*.*"
        "Frame/01_mcu/Libraries/STM32F4xx_StdPeriph_Driver/src/*.c"
        "Frame/02_hal/*.c"
        "Frame/03_chip_driver/*.c"
        "Frame/04_utils_cpp/*.cpp"
        "Frame/05_device_cpp/*.cpp"
        
 
        )

#编译优化等级
add_compile_options(${OPTIMIZATION}) 

#链接指令
add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${SREC_NAME}.map)
add_link_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork -specs=nosys.specs -specs=nano.specs)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-T ${LINKER_SCRIPT})

#链接静态库


#生成elf可执行文件
add_executable(${SREC_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

#Srec文件名
set(SREC_FILE ${PROJECT_BINARY_DIR}/${SREC_NAME}.srec)

#生成srec文件
add_custom_command(TARGET ${SREC_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Osrec $<TARGET_FILE:${SREC_NAME}.elf> ${SREC_FILE}
        COMMENT "Building ${SREC_FILE}")

