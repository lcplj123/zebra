ADD_SUBDIRECTORY(modules)
#------------common-----------------
SET(COMMON_SRC common.cpp)
ADD_LIBRARY(common_static STATIC ${COMMON_SRC})
ADD_LIBRARY(common_shared SHARED ${COMMON_SRC})
SET_TARGET_PROPERTIES(common_static PROPERTIES OUTPUT_NAME "common")
SET_TARGET_PROPERTIES(common_shared PROPERTIES OUTPUT_NAME "common")

#------------config-----------------
SET(CONFIG_SRC config.cpp)
ADD_LIBRARY(config_static STATIC ${CONFIG_SRC})
ADD_LIBRARY(config_shared SHARED ${CONFIG_SRC})
TARGET_LINK_LIBRARIES(config_static ifaddr_static)
SET_TARGET_PROPERTIES(config_static PROPERTIES OUTPUT_NAME "config")
SET_TARGET_PROPERTIES(config_shared PROPERTIES OUTPUT_NAME "config")

#-------------module manager-----------
SET(MODMGR_SRC modmgr.cpp)
ADD_LIBRARY(modmgr_static STATIC ${MODMGR_SRC})
ADD_LIBRARY(modmgr_shared SHARED ${MODMGR_SRC})
TARGET_LINK_LIBRARIES(modmgr_static dl ifaddr_static)
SET_TARGET_PROPERTIES(modmgr_static PROPERTIES OUTPUT_NAME "modmgr")
SET_TARGET_PROPERTIES(modmgr_shared PROPERTIES OUTPUT_NAME "modmgr")

#-------------mysqlconn-----------
SET(MYSQLCONN_SRC mysqlconn.cpp)
ADD_LIBRARY(mysqlconn_static STATIC ${MYSQLCONN_SRC})
ADD_LIBRARY(mysqlconn_shared SHARED ${MYSQLCONN_SRC})
TARGET_LINK_LIBRARIES(mysqlconn_static mysqlclient)
SET_TARGET_PROPERTIES(mysqlconn_static PROPERTIES OUTPUT_NAME "mysqlconn")
SET_TARGET_PROPERTIES(mysqlconn_shared PROPERTIES OUTPUT_NAME "mysqlconn")
LINK_DIRECTORIES(/usr/lib64/mysql)

#-------------getIP-----------
SET(IFADDR_SRC ifaddr.cpp)
ADD_LIBRARY(ifaddr_static STATIC ${IFADDR_SRC})
ADD_LIBRARY(ifaddr_shared SHARED ${IFADDR_SRC})
SET_TARGET_PROPERTIES(ifaddr_static PROPERTIES OUTPUT_NAME "ifaddr")
SET_TARGET_PROPERTIES(ifaddr_shared PROPERTIES OUTPUT_NAME "ifaddr")


#-------------zebra-------------------
SET(ZEBRA_SRC zebra.cpp)
ADD_EXECUTABLE(zebra ${ZEBRA_SRC})
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/src)
TARGET_LINK_LIBRARIES(zebra modmgr_static common_static config_static common_static mysqlconn_static curl)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


