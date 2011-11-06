--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   01:07:24 11/06/2011
-- Design Name:   
-- Module Name:   /home/daniel/coding/musasim/hardware/mkglue/tb_toplevel.vhd
-- Project Name:  mkglue
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: toplevel
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY tb_toplevel IS
END tb_toplevel;
 
ARCHITECTURE behavior OF tb_toplevel IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT toplevel
    PORT(
         led1 : OUT  std_logic;
         led2 : OUT  std_logic;
         switch : IN  std_logic;
         clock : IN  std_logic;
         cpuclock : OUT  std_logic;
			cpureset : OUT std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal switch : std_logic := '0';
   signal clock : std_logic := '0';

 	--Outputs
   signal led1 : std_logic;
   signal led2 : std_logic;
   signal cpuclock : std_logic;
	signal cpureset : std_logic;

   -- Clock period definitions
   constant clock_period : time := 40 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: toplevel PORT MAP (
          led1 => led1,
          led2 => led2,
          switch => switch,
          clock => clock,
          cpuclock => cpuclock,
			 cpureset => cpureset
        );

   -- Clock process definitions
   clock_process :process
   begin
		clock <= '0';
		wait for clock_period/2;
		clock <= '1';
		wait for clock_period/2;
   end process;
 
   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for clock_period*10;

      -- insert stimulus here 

      wait;
   end process;

END;
