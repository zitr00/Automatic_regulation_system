# The theme of the project

### A microcontroller-based driven system will be developed, which will perform the following functions: 
- Heating the system to a set temperature  "TSET" in a time ( "T.heating")

- Maintenance of these temperatures for a period of time timp "T.maintenance"

- System cooling gradually over a period of time "T.cooling"

### The user interface

- The system will have a 16x2 LCD on which the menu will be displayed, and during the running will be displayed the set temperature, the current temperature and the time remaining from the current stage  ("T.heating", "T.maintenance", "T.cooling"). 

- The menu of the system  will allow to change the following parameters: "TSET", "T.heating", "T.maintenance", "T.racire", "K.p", "K.i", "K.d" 

- Parameters  will be  saved  on   non-volatile memories. Restarting the system will not affect the saved parameters. 

- Navigation through  the menu of the equipment will be done through four buttons: "OK", "Cancel", "+", "-". 

- Optionally, it is possible to create an interface on PC for the display of a single graph of the temperature. 

### Temperature regulation
- Temperature control will be provided by a PID  type regulator

- The temperature sensor  used will be one of type LM35 or equivalent.

-The execution element of the system that will  ensure the heating of the  sensor will be an incandescent bulb c.c. of power > = 5W that will be able to reach a temperature of at least 50 °C 

- A relay or transistor shall be used to control the microcontroller turning the supply of this bulb on/off. 

### Disturbances 
- A  random  number generator (PRNG or TRNG) will be implemented 

- The system will have a disruptive generator that will affect the temperature measuring circuit  with random  values using the previously  implemented PRNG/TRNG module.

- Pressing the "OK" button while running a temperature control program will start the disturbance mode, then pressing it again will stop it. 

### Project video
- https://drive.google.com/file/d/1hDIfY1trIYdzggnsuAM-QJQR2-MU5qbz/view