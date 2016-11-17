# iRDash-Server
Connects to a running iRacing simulation and sends its telemetry data via USB to an Arduino board.
Currently only the car's telemetry data is sent, the slow moving data (lap times, other cars, etc.) is not.

Ardunio project to display the transmitted data on a TFT display:
https://github.com/UmmonPwr/iRDash_Client

To compile the solution you need to download the iRacing SDK from iRacing's forum and place it in the "irsdk" folder next to the folder where you downloaded "iRDash server".