// (c) Copyright 1995-2015 Xilinx, Inc. All rights reserved.
// 
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
// 
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
// 
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
// 
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
// 
// DO NOT MODIFY THIS FILE.


// IP VLNV: user.org:user:ov7670_top:1.0
// IP Revision: 6

(* X_CORE_INFO = "ov7670_top,Vivado 2014.4" *)
(* CHECK_LICENSE_TYPE = "design_1_ov7670_top_0_1,ov7670_top,{}" *)
(* DowngradeIPIdentifiedWarnings = "yes" *)
module design_1_ov7670_top_0_1 (
  clk,
  scl,
  sda,
  ov7670_reset,
  pwdn,
  xclk,
  pclk,
  href,
  vsync,
  data,
  M_AXIS_TREADY,
  M_AXIS_TVALID,
  M_AXIS_TLAST,
  M_AXIS_TUSER,
  M_AXIS_TDATA
);

(* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 signal_clock CLK" *)
input wire clk;
output wire scl;
inout wire sda;
(* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 ov7670_signal_reset RST" *)
output wire ov7670_reset;
output wire pwdn;
output wire xclk;
input wire pclk;
input wire href;
input wire vsync;
input wire [7 : 0] data;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TREADY" *)
input wire M_AXIS_TREADY;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TVALID" *)
output wire M_AXIS_TVALID;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TLAST" *)
output wire M_AXIS_TLAST;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TUSER" *)
output wire M_AXIS_TUSER;
(* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS TDATA" *)
output wire [23 : 0] M_AXIS_TDATA;

  ov7670_top inst (
    .clk(clk),
    .scl(scl),
    .sda(sda),
    .ov7670_reset(ov7670_reset),
    .pwdn(pwdn),
    .xclk(xclk),
    .pclk(pclk),
    .href(href),
    .vsync(vsync),
    .data(data),
    .M_AXIS_TREADY(M_AXIS_TREADY),
    .M_AXIS_TVALID(M_AXIS_TVALID),
    .M_AXIS_TLAST(M_AXIS_TLAST),
    .M_AXIS_TUSER(M_AXIS_TUSER),
    .M_AXIS_TDATA(M_AXIS_TDATA)
  );
endmodule
