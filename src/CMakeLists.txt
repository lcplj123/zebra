ADD_SUBDIRECTORY(modules)
#------------config-----------------
SET(CONFIG_SRC config.cpp)
ADD_LIBRARY(config_static STATIC ${CONFIG_SRC})
ADD_LIBRARY(config_shared SHARED ${CONFIG_SRC})
SET_TARGET_PROPERTIES(config_static PROPERTIES OUTPUT_NAME "config")
SET_TARGET_PROPERTIES(config_shared PROPERTIES OUTPUT_NAME "config")


#-------------zebra-------------------
SET(ZEBRA_SRC zebra.cpp)
ADD_EXECUTABLE(zebra ${ZEBRA_SRC})
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/src)
TARGET_LINK_LIBRARIES(zebra config_static)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

