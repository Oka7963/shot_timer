Function Block Diagram

<pre>



+------+            +--------+            +--------+
|debug |<--UART2--> |  STM32 | <--SPI1--> | ST3772 |
+------+            |  F103  |            +--------+
                    |        |            +--------------+
                    |        | <--INT0--> | sound sensor |
                    |        |            +--------------+
                    |        |            +----------+
                    |        | <--INT1--> |  Button  |
                    |        |            +----------+
                    |        |            +-----------+
                    |        | <--ADC?--> | arrow key |
                    |        |            +-----------+
                    |        |
                    +--------+




</pre>

State Machine (yeah, I'll swith to drawio...)
<pre>

                         start
                           |
                           V
  record  <--sel normal-- main  --sel conf--> config
  mode    --exit btn----> menu  <--exit btn-- menu

</pre>



# Summary

 A practice tool for IDPA
* record time for each shot with increment time
* show on a TFT LCD screen
* delay between button push and count time


## TODO:
* change shot count sensor to ADC pin
* random delay time for count start
* scroll throug the record
* store record in Flash/external SD
