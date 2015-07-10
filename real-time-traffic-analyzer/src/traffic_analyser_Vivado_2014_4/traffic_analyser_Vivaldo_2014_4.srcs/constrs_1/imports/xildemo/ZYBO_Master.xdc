##LEDs
#set_property PACKAGE_PIN M14 [get_ports {led[0]}]
#set_property PACKAGE_PIN M15 [get_ports {led[1]}]
#set_property PACKAGE_PIN G14 [get_ports {led[2]}]
#set_property PACKAGE_PIN D18 [get_ports {led[3]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {led[*]}]

##GPS SERIAL
set_property PACKAGE_PIN W20 [get_ports gps_rx]
set_property IOSTANDARD LVCMOS33 [get_ports gps_tx]

set_property PACKAGE_PIN V20 [get_ports gps_tx]
set_property IOSTANDARD LVCMOS33 [get_ports gps_rx]

##GSM SERIAL
set_property PACKAGE_PIN W19 [get_ports gsm_rx]
set_property IOSTANDARD LVCMOS33 [get_ports gsm_rx]

set_property PACKAGE_PIN W18 [get_ports gsm_tx]
set_property IOSTANDARD LVCMOS33 [get_ports gsm_tx]

# ov7670 camera ports
set_property PACKAGE_PIN H15 [get_ports ov7670_reset]
set_property IOSTANDARD LVCMOS33 [get_ports ov7670_reset]

set_property PACKAGE_PIN Y17 [get_ports ov7670_pwdn]
set_property IOSTANDARD LVCMOS33 [get_ports ov7670_pwdn]

set_property PACKAGE_PIN P14 [get_ports ov7670_pclk]
set_property IOSTANDARD LVCMOS33 [get_ports ov7670_pclk]
set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets ov7670_pclk]

set_property PACKAGE_PIN T15 [get_ports ov7670_vsync]
set_property IOSTANDARD LVCMOS33 [get_ports ov7670_vsync]

set_property PACKAGE_PIN T14 [get_ports ov7670_scl]
set_property IOSTANDARD LVCMOS33 [get_ports ov7670_scl]

set_property PACKAGE_PIN U14 [get_ports ov7670_sda]
set_property IOSTANDARD LVCMOS33 [get_ports ov7670_sda]

set_property PACKAGE_PIN V17 [get_ports ov7670_xclk]
set_property IOSTANDARD LVCMOS33 [get_ports ov7670_xclk]

set_property PACKAGE_PIN U15 [get_ports ov7670_href]
set_property IOSTANDARD LVCMOS33 [get_ports ov7670_href]

set_property PACKAGE_PIN T17 [get_ports {ov7670_data[0]}]
set_property PACKAGE_PIN J16 [get_ports {ov7670_data[1]}]
set_property PACKAGE_PIN U17 [get_ports {ov7670_data[2]}]
set_property PACKAGE_PIN W16 [get_ports {ov7670_data[3]}]
set_property PACKAGE_PIN V13 [get_ports {ov7670_data[4]}]
set_property PACKAGE_PIN V12 [get_ports {ov7670_data[5]}]
set_property PACKAGE_PIN V18 [get_ports {ov7670_data[6]}]
set_property PACKAGE_PIN R14 [get_ports {ov7670_data[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ov7670_data[*]}]