# utilities

## Parameters
- utility_type:
    - alarm
        - operation: check
    - battery
    - update
        - operation: check
        - operation: update
    - date
    - time
    - light
    - volume
    - weather
- time: \<*empty?!*>
- light:
    - operation:
        - off
        - set_hsl
    - hsl:
        - 0: \<*number 0-330*>
        - 1: \<*number 0-100*>
        - 1: \<*number 0-100*>
    - room: \<*empty?!*>
- volume:
    - operation:
        - max
        - mid
        - min
        - mute
- weather:
    - main:
        - sunny
        - cloudy
    - temp: \<*float with 2 digits; sperator is a dot*>

