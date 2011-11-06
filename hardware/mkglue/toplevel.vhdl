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
use IEEE.NUMERIC_STD.ALL;

entity toplevel is
    Port ( led1 : out  STD_LOGIC;
           led2 : out  STD_LOGIC;
           switch : in  STD_LOGIC;
           clock : in  STD_LOGIC;
			  cpuclock: out STD_LOGIC);
end toplevel;

architecture Behavioral of toplevel is
	
	signal counter : STD_LOGIC_VECTOR(21 downto 0) := (others => '0');

begin

led1 <= counter(21);
led2 <= '1';

process(clock)
begin
	if(rising_edge(clock)) then
		counter <= STD_LOGIC_VECTOR(unsigned(counter) + 1);
	end if;
end process;

cpuclock <= counter(3);

end Behavioral;

