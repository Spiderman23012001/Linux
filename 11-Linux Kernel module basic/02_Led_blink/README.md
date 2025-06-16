# Blink Led
- GPFSELn: GPIO Pin Function Select as Input, Output,...
- GPSETn: GPIO Set as High - GPIO Pin Output Set Register
- GPCLRn: GPIO Clear as Low - GPIO Pin Output Clear Register
- GPLEVn: Read status level of Pin - GPIO Pin Level Register
- GPEDSn: GPIO Event Detect Status Register

## How to blink led ?

### Step 1: Define GPIO_BASE_ADDR, GPIO_SIZE, LED_PIN, GPIO_FSELn, GPIO_GPSETn, GPIO_GPCLRn
### Step 2: Initialize GPIO - get GPIO_BASE
### Step 3: Set GPIO Pin Function Select as Output
### Step 4: Set GPIO Pin Output Set Register
### Step 5: Exit function 
### Step 6: Clear GPIO Pin Output Clear Register

