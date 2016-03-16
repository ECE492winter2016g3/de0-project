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
entity stepper_driver is
	port (
		-- Avalon clock / reset interface
		clk: in std_logic;
		rst: in std_logic;
		
		-- Avalon memory mapped interface
		write: inout std_logic;
		writedata: inout std_logic_vector(31 downto 0);
		address: inout std_logic_vector(0 downto 0);
		
		-- Avalon condiuts
		c_ena: out std_logic;
		c_enb: out std_logic;
		c_ain1: out std_logic;
		c_ain2: out std_logic;
		c_bin1: out std_logic;
		c_bin2: out std_logic
	);
end entity;

architecture avalon of stepper_driver is
	-- Polarity for coils
	signal coila: std_logic;
	signal coilb: std_logic;

	-- Enable for coils
	signal coila_enable: std_logic;
	signal coilb_enable: std_logic;
	
	-- Current step index
	type STEP_NUMBER is (a, b, c, d, e, f, g, h);
	signal cur_step: STEP_NUMBER := a;
	signal step_speed: std_logic_vector(31 downto 0) := x"001FFFFF";
	
	-- Number of steps remaining
	signal num_step: signed(31 downto 0) := x"FFFFFF39"; --x"000000C8";
	signal step_divisor: std_logic_vector(31 downto 0) := x"00000000";
begin
	process (clk) is
	begin
		if (clk'event and clk = '1') then
--			-- Write handling
--			if (write = '1') then
--				coila <= writedata(0);
--				coila_enable <= writedata(1);
--				coilb <= writedata(2);
--				coilb_enable <= writedata(3);
--			end if;

			-- Write handling
			if (write = '1') then
				if (address(0) = '0') then
					num_step <= num_step + signed(writedata);
				else
					step_speed <= writedata;
				end if;
			end if;
			
			-- Motor rotation
			if (num_step /= (num_step'range => '0')) then
				if (step_divisor = (num_step'range => '0')) then
					-- Take a step
					step_divisor <= step_speed;
					if (num_step(31) = '1') then
						num_step <= num_step + 1;
						case cur_step is
							when a => cur_step <= h;
							when b => cur_step <= a;
							when c => cur_step <= b;
							when d => cur_step <= c;
							when e => cur_step <= d;
							when f => cur_step <= e;
							when g => cur_step <= f;
							when h => cur_step <= g;
						end case;
					else
						num_step <= num_step - 1;
						case cur_step is
							when a => cur_step <= b;
							when b => cur_step <= c;
							when c => cur_step <= d;
							when d => cur_step <= e;
							when e => cur_step <= f;
							when f => cur_step <= g;
							when g => cur_step <= h;
							when h => cur_step <= a;
						end case;
					end if;
				else
					step_divisor <= std_logic_vector(unsigned(step_divisor) - 1);
				end if;
			end if;
		end if;
	end process;
	
	-- Convert step into polarities
	process (cur_step) is
	begin
		case cur_step is
			when a =>
				coila <= '0';
				coila_enable <= '1';
				coilb <= '0';
				coilb_enable <= '0';
			when b =>
				coila <= '0';
				coila_enable <= '1';
				coilb <= '0';
				coilb_enable <= '1';
			when c =>
				coila <= '0';
				coila_enable <= '0';
				coilb <= '0';
				coilb_enable <= '1';
			when d =>
				coila <= '1';
				coila_enable <= '1';
				coilb <= '0';
				coilb_enable <= '1';
			when e =>
				coila <= '1';
				coila_enable <= '1';
				coilb <= '0';
				coilb_enable <= '0';
			when f =>
				coila <= '1';
				coila_enable <= '1';
				coilb <= '1';
				coilb_enable <= '1';
			when g =>
				coila <= '0';
				coila_enable <= '0';
				coilb <= '1';
				coilb_enable <= '1';
			when h =>
				coila <= '0';
				coila_enable <= '1';
				coilb <= '1';
				coilb_enable <= '1';
		end case;
	end process;
	
	-- Enable handling, can just be passed directly to a
	-- condiut for the chip to handle
	c_ena <= coila_enable;
	c_enb <= coilb_enable;
	
	-- Coila polarity
	process (coila) is
	begin
		if (coila = '0') then
			c_ain1 <= '1';
			c_ain2 <= '0';
		else
			c_ain1 <= '0';
			c_ain2 <= '1';
		end if;
	end process;

	-- Coilb polarity
	process (coilb) is
	begin
		if (coilb = '0') then
			c_bin1 <= '1';
			c_bin2 <= '0';
		else
			c_bin1 <= '0';
			c_bin2 <= '1';
		end if;
	end process;
end architecture avalon;