# iRDash-Server
Connects to a running iRacing simulation and sends its telemetry data via USB to an Arduino board.
Currently only the car's telemetry data is sent, the slow moving data (lap times, other cars, etc.) is not.

Uses iRacing's SDK to connect to the sim.
To compile the solution you need to download the iRacing SDK separately and place it in the "irsdk" folder next to the folder where you downloaded "iRDash server".
