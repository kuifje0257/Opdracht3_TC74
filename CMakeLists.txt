cmake_minimum_required (VERSION 2.6)

project (Opdracht3)
include_directories(paho.mqtt.c)

add_executable(Opdracht3 Publish.c)
TARGET_LINK_LIBRARIES(Opdracht3 paho-mqtt3c)