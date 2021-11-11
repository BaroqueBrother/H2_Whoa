# H2_Whoa
## About H2Whoa
This is H2whoa
An indoor plant watering and room monitoring system.
Here's what I used to put it all together.
BME280
Reads humidity, pressure, and, temperature
Measurements are either triggered by the user manually or performed automatically in regular intervals
The BME can be used for:
-skin detection
-Home automation control
-Indoor navigation like going from one floor to another
-In my project I'll be using it to read temperature, humidity, and barometric pressure
-In general, a rising barometer means improving weather
-In general, a falling barometer means worsening weather
-When atmospheric pressure drops suddenly, this usually indicates that a storm is on its way.
-When atmospheric pressure remains steady, there will likely be no immediate change in the weather
Moisture sensor
Uses the capacitive method to measure the moisture in the soil
Made from non-corrosive materials and outputs an analog signal and based on that reading we can gauge our soil moisture level
Dust sensor
-Gauges the dust levels through a process using heat and light
-Gives a good indication of the air quality in an environment by measuring the dust concentration
-The unit must be Kept it upright to operate efficiently
The Grove air quality sensor
-Monitors for harmful gases such as carbon monoxide, alcohol, formaldehyde and so on
-Although it doesn’t output the presence of WHICH pollutant has been detected, it registers a reading indicating the presence of 1, some, or all, the monitored gases.
Water pump and relay
-The pump used for watering our plant requires more power than the Argon can supply directly, so a relay to step up to the pump requirement is used
Just like humans, for plants to grow and thrive, the environment of which it lives will prove the quality of life.
With the data collected from the chosen environmental sensors in this project, I'm able to then represent accurate , useful, information that helps make informed decisions concerning the ideal environment for the plant life.````
How I chose to represent my data is through an easy to read online visual dashboard. It provides updated information down to the minute.
OLED
Grove - OLED Display - I2c
In addition to the online dashboard, I've installed a Grove OLED display with sensor readings
Initially I wanted to make each reading displayed differently just for the sake of diversity. By doing that, my dashboard looked "cool" but it took more effort to discern what I was actually looking at and what any of it meant. I revised it to be more natural, when we look at our car dashboards the temperature, speed, and other gauges are mostly circular, a low side, high side, and typically we desire to be in between those same with the dashboard I've created.
