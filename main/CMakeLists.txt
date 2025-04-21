set(WIFI_DIR ../include/wifi_connectivity)
set(MQTT_DIR ../include/mqtt)
set(MQTT_CPP ../include/mqtt/mqtt.cpp)
set(WIFI_CPP ../include/wifi_connectivity/wifi.cpp)
set(HELPER_CPP ../main/helper.cpp)
  
idf_component_register(SRCS main.cpp  ${MQTT_CPP} ${WIFI_CPP}
                    INCLUDE_DIRS "."  ${MQTT_DIR} ${WIFI_DIR})

