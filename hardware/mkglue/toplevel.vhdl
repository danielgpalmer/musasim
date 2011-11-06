----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    23:37:08 11/05/2011 
-- Design Name: 
-- Module Name:    toplevel - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity toplevel is
    Port ( led1 : out  STD_LOGIC;
           led2 : out  STD_LOGIC;
           switch : in  STD_LOGIC;
           clock : in  STD_LOGIC;
			  cpuclock: out STD_LOGIC);
end toplevel;

architecture Behavioral of toplevel is

begin

led1 <= '1';
led2 <= '1';
cpuclock <= clock;

end Behavioral;

