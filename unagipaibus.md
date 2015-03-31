# Bus Standard for the UnagiPai #

## Intro ##

The board will have 5 slots;
1 CPU slot
4 Card slots

All connectors are DIN41612 C cases with all 96 pins populated.

The address space is broken up in 8 chipselects.

## CPU Slot ##

  * VCC - 2
  * GND - 2

  * 4

  * #AS
  * #UDS
  * #LDS
  * R/#W
  * #DTACK
  * #BG
  * #BR
  * CLK
  * #HALT
  * #RESET
  * #AVEC
  * #BERR
  * #IPL2
  * #IPL1
  * #IPL0
  * FC2
  * FC1
  * FC0

  * 18

  * Address - A1->A23 - 23
  * Data - D0->D15 - 16

61

|     |  _**A**_     |  _**B**_  | _**C**_    |
|:----|:-------------|:----------|:-----------|
| _**1**_  | D15    | RESERVED | A23   |
| _**2**_  | D14    | RESERVED | A22   |
| _**3**_ | D13    | RESERVED | A21   |
|_**4**_  | D12    | RESERVED | A20   |
|_**5**_  | D11    |RESERVED  | A19   |
| _**6**_ | D10    | RESERVED | A18   |
| _**7**_  | D9     | RESERVED | A17   |
| _**8**_  | D8     | RESERVED | A16   |
| _**9**_ | D7     | RESERVED | A15   |
| _**10**_ | D6     | RESERVED | A14   |
| _**11**_ | D5     | RESERVED | A13   |
| _**12**_ | D4     | RESERVED | A12   |
| _**13**_ | D3     | RESERVED | A11   |
| _**14**_| D2     | RESERVED | A10   |
| _**15**_ | D1     | RESERVED| A9    |
| _**16**_ | D0     | RESERVED | A8    |
| _**17**_ | #AS    | RESERVED | A7    |
| _**18**_ | #UDS   | RESERVED | A6    |
| _**19**_ | #LDS   | RESERVED | A5    |
| _**20**_ | R/#W   | RESERVED | A4    |
| _**21**_ | #DTACK | A23 | A3    |
| _**22**_ | #BG    | A22 | A2    |
| _**23**_ | #BR    | A21 | A1    |
| _**24**_ | CLK    | RESERVED | FC0   |
| _**25**_ | #HALT  |  RESERVED| FC1   |
| _**26**_ | #RESET | RESERVED | FC2   |
| _**27**_ | #AVEC  | RESERVED | #IPL0 |
| _**28**_ | #BERR  | RESERVED | #IPL1 |
| _**29**_ |        | RESERVED | #IPL2 |
| _**30**_ |        | RESERVED |       |
| _**31**_ | GND    | GND | GND   |
|  _**32**_ | VCC    | VCC | VCC   |


## Card slot ##

### Power ###

  * VCC x 2
  * GND x 2

  * Total; 4

### Card specific signals ###

  * #PRESENT**<?>**
  * #IRQ0**<?>**
  * #IRQ0ACK**<?>**
  * #IRQ1**<?>**
  * #IRQ1ACK**<?>**
  * #CS0**<?>**
  * #CS1**<?>**
  * #BR0**<?>**
  * #BG0**<?>**
  * #BR1**<?>**
  * #BG1**<?>**

  * Total; 11


  * #AS
  * #UDS
  * #LDS
  * #RW
  * #DTACK
  * CCLK -- CPU Clock
  * MCLK -- Main Clock

  * #RESET
  * #AVEC
  * #BERR
  * FC2
  * FC1
  * FC0

  * 12

  * Address - A1->A20 - 20
  * Data - D0->D15 - 16

|         |  _**A**_    | _**B**_ |  _**C**_            |
|:--------|:------------|:--------|:--------------------|
|  _**1**_  | D15       | RESERVED  | #PRESENT**<?>** |
|  _**2**_  | D14       | RESERVED | #CS1**<?>**           |
|  _**3**_  | D13       | RESERVED  | #CS0**<?>**           |
|  _**4**_  | D12       | RESERVED   | A20           |
|  _**5**_  | D11       | RESERVED  | A19           |
|  _**6**_  | D10       | RESERVED  | A18           |
|  _**7**_  | D9        |  RESERVED | A17           |
|  _**8**_  | D8        |   RESERVED| A16           |
|  _**9**_  | D7        | RESERVED  | A15           |
|  _**10**_ | D6        |  RESERVED | A14           |
|  _**11**_ | D5        |  RESERVED | A13           |
|  _**12**_ | D4        |  RESERVED | A12           |
|  _**13**_ | D3        |  RESERVED | A11           |
|  _**14**_ | D2        |  RESERVED | A10           |
|  _**15**_ | D1        |  RESERVED | A9            |
|  _**16**_ | D0        |  RESERVED | A8            |
|  _**17**_ | #AS       |  RESERVED | A7            |
|  _**18**_ | #UDS      |  RESERVED | A6            |
|  _**19**_ | #LDS      | RESERVED | A5            |
|  _**20**_ | R/#W      | RESERVED | A4            |
|  _**21**_ | #DTACK    | **A23** | A3            |
|  _**22**_ | #BG1**<?>** | **A22** | A2            |
|  _**23**_ | #BR1**<?>** | **A21** | A1            |
|  _**24**_ | CCLK      |  RESERVED | FC0           |
|  _**25**_ | MCLK      |  RESERVED | FC1           |
|  _**26**_ | #RESET    | RESERVED | FC2           |
|  _**27**_ | #AVEC     | RESERVED | IRQ1ACK**<?>**  |
|  _**28**_ | #BERR     | RESERVED | IRQ1**<?>**     |
|  _**29**_ | #BG0**<?>** | RESERVED | IRQ0ACK**<?>**  |
|  _**30**_ | #BR0**<?>** | RESERVED | IRQ0**<?>**     |
|  _**31**_ | GND       | GND | GND           |
|  _**32**_ | VCC       | VCC | VCC           |

A21 - A23 are only used for cards that bus master and are not needed on cards that don't. This should make routing two layer cards easier.

## Glue logic ##

### Card presence detect ###

  * #PRESENT0
  * #PRESENT1
  * #PRESENT2
  * #PRESENT3

### Chip select ###

  * #AS
  * A21 - A23
  * #CS00
  * #CS10
  * #CS01
  * #CS11
  * #CS02
  * #CS12
  * #CS03
  * #CS13

### Bus grant ###

  * MCLK
  * #BG
  * #BR
  * #BR00
  * #BG00
  * #BR10
  * #BG10
  * #BR01
  * #BG01
  * #BR11
  * #BG11
  * #BR02
  * #BG02
  * #BR12
  * #BG12
  * #BR03
  * #BG03
  * #BR13
  * #BG13

### Interrupts ###

  * #AS
  * #IPL2
  * #IPL1
  * #IPL0
  * #IRQ00
  * #IRQ0ACK0
  * #IRQ10
  * #IRQ1ACK0
  * #IRQ01
  * #IRQ0ACK1
  * #IRQ11
  * #IRQ1ACK1
  * #IRQ02
  * #IRQ0ACK2
  * #IRQ12
  * #IRQ1ACK2
  * #IRQ03
  * #IRQ0ACK3
  * #IRQ13
  * #IRQ1ACK3
  * FC2
  * FC1
  * FC0
  * A1 - A3

### CPU Control ###

  * CCLK
  * #HALT
  * #RESET