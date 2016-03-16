-- File: dc_driver.vhd
-- Author: Mark Langen
-- Description: 
--   Avalon interface component that handles 
--   driving of the two dc drive motors for
--   our robot platform through a dual bridge chip.

-- Commonly imported packages:
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.VITAL_Primitives.all;

-- Main dc_driver component
entity dc_driver is
	port (
		-- Avalon clock / reset interface
		clk: in std_logic;
		rst: in std_logic;
		
		-- Avalon memory mapped interface
		write: in std_logic;
		writedata: in std_logic_vector(31 downto 0);
		
		-- Read back
		read: in std_logic;
		readdata: out std_logic_vector(31 downto 0);
		
		-- RW Address
		address: in std_logic_vector(0 downto 0);
		
		-- Avalon condiuts
		c_pwma: out std_logic;
		c_pwmb: out std_logic;
		c_ain1: out std_logic;
		c_ain2: out std_logic;
		c_bin1: out std_logic;
		c_bin2: out std_logic;
		c_stby: out std_logic
	);
end entity;

-- Architecture
architecture impl of dc_driver is
	-- Motor states
	type MOTOR_STATE is (cw, ccw, brake);
	signal count: std_logic_vector(31 downto 0) := x"22348765";
	signal motora: MOTOR_STATE := brake;
	signal motorb: MOTOR_STATE := brake;
begin
	-- Constant signals
	c_pwma <= '1';
	c_pwmb <= '1';
	c_stby <= '1';

	-- Handle reading by avalon bus
	process (clk) is
	begin
		if (clk'event and clk = '1') then
			if (read = '1') then
				readdata <= count;
			end if;
		end if;
	end process;
	
	-- Handle writing by avalon bus
	process (clk) is
	begin
		if (clk'event and clk = '1' and write = '1') then
			count(31 downto 16) <= std_logic_vector(unsigned(count(31 downto 16)) + 1);
			--if (address(0) = '0') then
				-- Motor control for motor a
				case writedata(1 downto 0) is
					when "11" => motora <= cw;
					when "10" => motora <= ccw;
					when others => motora <= brake;
				end case;
				
				-- Motor control for motor b
				case writedata(3 downto 2) is
					when "11" => motorb <= cw;
					when "10" => motorb <= ccw;
					when others => motorb <= brake;
				end case;
			--else
				-- TODO:
				-- enables maybe?
			--end if;
		end if;
	end process;
	
	-- Handle output to motors
	process (motora) is
	begin
		case motora is
			when cw =>
				c_ain1 <= '0';
				c_ain2 <= '1';
			when ccw =>
				c_ain1 <= '1';
				c_ain2 <= '0';
			when brake =>
				c_ain1 <= '0';
				c_ain2 <= '0';
		end case;
	end process;
	
	process (motorb) is
	begin
		case motorb is 
			when cw =>
				c_bin1 <= '0';
				c_bin2 <= '1';
			when ccw =>
				c_bin1 <= '1';
				c_bin2 <= '0';
			when brake =>
				c_bin1 <= '0';
				c_bin2 <= '0';
		end case;
	end process;
end architecture impl;