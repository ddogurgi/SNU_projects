// memwb_reg.v
// This module is the MEM/WB pipeline register.


module memwb_reg #(
  parameter DATA_WIDTH = 32
)(
  // TODO: Add flush or stall signal if it is needed

  //////////////////////////////////////
  // Inputs
  //////////////////////////////////////
  input clk,

  input [DATA_WIDTH-1:0] mem_pc_plus_4,

  // wb control
  input [1:0] mem_jump,
  input mem_memtoreg,
  input mem_regwrite,
  
  input [DATA_WIDTH-1:0] mem_readdata,
  input [DATA_WIDTH-1:0] mem_alu_result,
  input [4:0] mem_rd,
  
  //////////////////////////////////////
  // Outputs
  //////////////////////////////////////
  output reg [DATA_WIDTH-1:0] wb_pc_plus_4,

  // wb control
  output reg [1:0] wb_jump,
  output reg wb_memtoreg,
  output reg wb_regwrite,
  
  output reg [DATA_WIDTH-1:0] wb_readdata,
  output reg [DATA_WIDTH-1:0] wb_alu_result,
  output reg [4:0] wb_rd
);

// TODO: Implement MEM/WB pipeline register module
  always @(posedge clk) begin
    wb_pc_plus_4 <= mem_pc_plus_4;
    wb_jump <= mem_jump;
    wb_memtoreg <= mem_memtoreg;
    wb_regwrite <= mem_regwrite;
    wb_readdata <= mem_readdata;
    wb_alu_result <= mem_alu_result;
    wb_rd <= mem_rd;
  end
endmodule
