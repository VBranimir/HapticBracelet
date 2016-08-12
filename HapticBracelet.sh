#!/bin/bash

### Set initial time of file
LTIME=`stat -c %Z /tmp/input.log`
mqttBroker="MQTT_BROKER_IP"



HAPTIC() {

input=$(tail -n 1 /tmp/input.log);
hTime=$(date +%H":"%M);
hSens="1"; #add sensor input stream
hRply="12";#add sensor input stream
hFcast="23";#add sensor input stream

	#time
if [[ $input -eq "1" ]];
	then
		mosquitto_pub -h "$mqttBroker" -t "/haptic/time" -m "$hTime";
		echo "one click detected, current time is: $hTime";
elif [[ $input -eq "2" ]];
	then
		hTemp=$(curl http://pljusak.com/retfala/wx.htm | grep temperatura= | cut -d "=" -f 2 | cut -d "." -f 1);  # link to w. forecast
		mosquitto_pub -h "$mqttBroker" -t "/haptic/temp" -m "$hTemp";
		echo "two clicks detected, outside temperature is: $hTemp °c ";
elif [[ $input -eq "3" ]];
	then
		mosquitto_pub -h "$mqttBroker" -t "/haptic/sens" -m "$hSens";
		echo $hSens;
elif [[ $input -eq "4" ]];
	then
		mosquitto_pub -h "$mqttBroker" -t "/haptic/rply" -m "$hRply";
		echo $hRply;
else
	echo $input;
fi

}


while true    
do
   ATIME=`stat -c %Z /tmp/input.log`

   if [[ "$ATIME" != "$LTIME" ]]
   then    
       LTIME=$ATIME
       HAPTIC
   fi
   sleep 5
done
