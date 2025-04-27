set(WIFI_DIR ../include/wifi_connectivity)
set(MQTT_DIR ../include/mqtt)
set(MQTT_CPP ../include/mqtt/mqtt.cpp)
set(WIFI_CPP ../include/wifi_connectivity/wifi.cpp)
set(PANEL_FUNCTIONS_CPP PanelFunctions.cpp)
set(CONSTANTS_CPP constants.cpp)
set(HELPER_CPP ../main/helper.cpp)
  
idf_component_register(SRCS main.cpp  ${MQTT_CPP} ${WIFI_CPP} ${PANEL_FUNCTIONS_CPP} ${CONSTANTS_CPP}
                    INCLUDE_DIRS "."  ${MQTT_DIR} ${WIFI_DIR})

