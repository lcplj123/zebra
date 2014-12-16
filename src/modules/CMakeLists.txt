INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

#----------------cpu---------------------------
SET(MODULE_CPU_SRC module_cpu.cpp)
ADD_LIBRARY(module_cpu_static STATIC ${MODULE_CPU_SRC})
ADD_LIBRARY(module_cpu_shared SHARED ${MODULE_CPU_SRC})
SET_TARGET_PROPERTIES(module_cpu_static PROPERTIES OUTPUT_NAME "module_cpu")
SET_TARGET_PROPERTIES(module_cpu_shared PROPERTIES OUTPUT_NAME "module_cpu")

#----------------memory---------------------------
SET(MODULE_MEM_SRC module_mem.cpp)
ADD_LIBRARY(module_mem_static STATIC ${MODULE_MEM_SRC})
ADD_LIBRARY(module_mem_shared SHARED ${MODULE_MEM_SRC})
SET_TARGET_PROPERTIES(module_mem_static PROPERTIES OUTPUT_NAME "module_mem")
SET_TARGET_PROPERTIES(module_mem_shared PROPERTIES OUTPUT_NAME "module_mem")

#----------------traffic---------------------------
SET(MODULE_TRAFFIC_SRC module_traffic.cpp)
ADD_LIBRARY(module_traffic_static STATIC ${MODULE_TRAFFIC_SRC})
ADD_LIBRARY(module_traffic_shared SHARED ${MODULE_TRAFFIC_SRC})
SET_TARGET_PROPERTIES(module_traffic_static PROPERTIES OUTPUT_NAME "module_traffic")
SET_TARGET_PROPERTIES(module_traffic_shared PROPERTIES OUTPUT_NAME "module_traffic")

#----------------disk---------------------------
SET(MODULE_DISK_SRC module_disk.cpp)
ADD_LIBRARY(module_disk_static STATIC ${MODULE_DISK_SRC})
ADD_LIBRARY(module_disk_shared SHARED ${MODULE_DISK_SRC})
SET_TARGET_PROPERTIES(module_disk_static PROPERTIES OUTPUT_NAME "module_disk")
SET_TARGET_PROPERTIES(module_disk_shared PROPERTIES OUTPUT_NAME "module_disk")

#----------------process---------------------------
SET(MODULE_PROCESS_SRC module_process.cpp)
ADD_LIBRARY(module_process_static STATIC ${MODULE_PROCESS_SRC})
ADD_LIBRARY(module_process_shared SHARED ${MODULE_PROCESS_SRC})
SET_TARGET_PROPERTIES(module_process_static PROPERTIES OUTPUT_NAME "module_process")
SET_TARGET_PROPERTIES(module_process_shared PROPERTIES OUTPUT_NAME "module_process")

#----------------load---------------------------
SET(MODULE_LOAD_SRC module_load.cpp)
ADD_LIBRARY(module_load_static STATIC ${MODULE_LOAD_SRC})
ADD_LIBRARY(module_load_shared SHARED ${MODULE_LOAD_SRC})
SET_TARGET_PROPERTIES(module_load_static PROPERTIES OUTPUT_NAME "module_load")
SET_TARGET_PROPERTIES(module_load_shared PROPERTIES OUTPUT_NAME "module_load")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/modules)
