# water-quality-monitoring-system

This project is to propose a system to monitor the quality of fish
pond water to control water pollution and various other factors with the assistance of IoT. The
depth of the water pollution can be identified by estimating the water parameters like turbidity,
total dissolved solids, variance in pH, etc... as the contrast in the values of these parameters
implies the presence of pollutants. These parameters are detected by chemical laboratory tests,
where the equipment is always stationary and samples are to be taken to the laboratories. Thus,
it is a manual system with a tedious and time-consuming process. Along with identifying the
degree of pollutants, many other factors must be considered. So that the farmers can take
necessary actions to avoid the death of fish. These sensors accept the data using various sensors
and send these collected data to a microcontroller for processing. The system utilizes the
features provided by the Cloud. The data collected is transferred to the cloud and there the data
processing functionalities are performed. The analysed data is made available to the farmers
and whenever the value monitored breaks the threshold the system provides a notification
through a mobile phone. Also, water pumps accompany the system in which one of the pumps
takes water out when the turbidity and total dissolved solids value goes above the maximum
8
threshold and the other takes water in when these values decrease. The farmers will be notified
about the changes that occur in the values of each parameter we are collecting. The suggested
project utilizes ESP32, which is an open-source development kit and firmware designed for
developing IoT products. It consists of firmware that operates on the ESP32 Wi-Fi system-onchip developed by Espressif Systems, and hardware that is built on the ESP-12 module. Sensors
are connected with ESP32 Microcontroller and it needs to be programmed. It is done with the
help of Arduino IDE. After that, the power is supplied to the prototype by using a type B USB
connector. This efficient system helps the farmers to monitor and manage their fish farm. It will
overcome the limitation of the existing system by reducing the time and labour.
