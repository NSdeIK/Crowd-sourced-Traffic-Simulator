`timescale 1ns / 1ps

module ov7670_top(
input   clk,
output  scl,
inout   sda,
output  ov7670_reset,
output  pwdn,
output  xclk,
input   pclk,
input   href,
input   vsync,
input   [7:0] data,
input   M_AXIS_TREADY,
output  M_AXIS_TVALID,
output  M_AXIS_TLAST,
output  M_AXIS_TUSER,
output  [23:0] M_AXIS_TDATA);

// http://hamsterworks.co.nz/mediawiki/index.php/Zedboard_OV7670
ov7670_controller controller(.clk(clk),
                             .sioc(scl),
                             .siod(sda),
                             .reset(ov7670_reset),
                             .pwdn(pwdn),
                             .xclk(xclk));

OV7670_CAPTURE_AXI_VDMA_STREAM ov7670_axi(.pclk(pclk),
                                          .href(href),
                                          .vsync(vsync),
                                          .data(data),
                                          .M_AXIS_TREADY(M_AXIS_TREADY),
                                          .M_AXIS_TVALID(M_AXIS_TVALID),
                                          .M_AXIS_TLAST(M_AXIS_TLAST),
                                          .M_AXIS_TUSER(M_AXIS_TUSER),
                                          .M_AXIS_TDATA(M_AXIS_TDATA));

endmodule
