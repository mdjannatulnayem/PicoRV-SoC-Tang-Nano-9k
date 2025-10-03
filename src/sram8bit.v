
module sram8
  #(
    parameter SRAM_ADDR_WIDTH = 11,
    parameter MEM_INIT_FILE = ""
    )
   (
    input                         clk,
    input                         reset_n,
    input                         ce, 
    input                         wre,
    input [SRAM_ADDR_WIDTH - 1:0] addr,
    input [7:0]                   data_in,
    output [7:0]                  data_out
    );
   
   reg [7:0]     mem[(1 << SRAM_ADDR_WIDTH) - 1:0];
   reg [7:0]     data_out_reg;

   initial begin
      if (MEM_INIT_FILE != "") begin
         $readmemh(MEM_INIT_FILE, mem);
      end
   end

   assign data_out = data_out_reg;

   always @(posedge clk or negedge reset_n)
     if (~reset_n)
       data_out_reg <= 0;
     else
       if (ce & !wre)
         data_out_reg <= mem[addr];

   always @(posedge clk)
     if (ce & wre)
       mem[addr] <= data_in;
   
endmodule // sram8
