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



SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/modules)
