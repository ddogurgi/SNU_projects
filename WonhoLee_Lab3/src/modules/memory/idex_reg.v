// idex_reg.v
// This module is the ID/EX pipeline register.


module idex_reg #(
  parameter DATA_WIDTH = 32
)(
  // TODO: Add flush or stall signal if it is needed
  input flush,
  input stall,
  //////////////////////////////////////
  // Inputs
  //////////////////////////////////////
  input clk,

  input [DATA_WIDTH-1:0] id_PC,
  input [DATA_WIDTH-1:0] id_pc_plus_4,

  // ex control
  input [1:0] id_jump,
  input id_branch,
  input [1:0] id_aluop,
  input id_alusrc,
  input [1:0] id_ui, //new ctrl sig for aui, luipc in Lab3
  // mem control
  input id_memread,
  input id_memwrite,

  // wb control
  input id_memtoreg,
  input id_regwrite,

  input [DATA_WIDTH-1:0] id_sextimm,
  input [6:0] id_funct7,
  input [2:0] id_funct3,
  input [DATA_WIDTH-1:0] id_readdata1,
  input [DATA_WIDTH-1:0] id_readdata2,
  input [4:0] id_rs1,
  input [4:0] id_rs2,
  input [4:0] id_rd,
  input [DATA_WIDTH-1:0] id_branch_predict,
  //////////////////////////////////////
  // Outputs
  //////////////////////////////////////
  output reg [DATA_WIDTH-1:0] ex_PC,
  output reg [DATA_WIDTH-1:0] ex_pc_plus_4,

  // ex control
  output reg ex_branch,
  output reg [1:0] ex_aluop,
  output reg ex_alusrc,
  output reg [1:0] ex_jump,
  output reg [1:0] ex_ui, //new ctrl sig for aui, luipc in Lab3
  // mem control
  output reg ex_memread,
  output reg ex_memwrite,

  // wb control
  output reg ex_memtoreg,
  output reg ex_regwrite,

  output reg [DATA_WIDTH-1:0] ex_sextimm,
  output reg [6:0] ex_funct7,
  output reg [2:0] ex_funct3,
  output reg [DATA_WIDTH-1:0] ex_readdata1,
  output reg [DATA_WIDTH-1:0] ex_readdata2,
  output reg [4:0] ex_rs1,
  output reg [4:0] ex_rs2,
  output reg [4:0] ex_rd,
  output reg [DATA_WIDTH-1:0] ex_branch_predict
);

// TODO: Implement ID/EX pipeline register module
  always @(posedge clk) begin
    ex_PC <= id_PC;
    ex_pc_plus_4 <= id_pc_plus_4;
    ex_sextimm <= id_sextimm;
    ex_funct7 <= id_funct7;
    ex_funct3 <= id_funct3;
    ex_readdata1 <= id_readdata1;
    ex_readdata2 <= id_readdata2;
    ex_rs1 <= id_rs1;
    ex_rs2 <= id_rs2;
    ex_rd <= id_rd;
    ex_branch_predict <= id_branch_predict;
    if((flush == 1)||(stall == 1)) begin
      ex_branch <= 0;
      ex_aluop <= 2'b00;
      ex_alusrc <= 0;
      ex_jump <= 2'b00;
      ex_memread <= 0;
      ex_memwrite <= 0;
      ex_memtoreg <= 0;
      ex_regwrite <= 0;
      ex_ui <= 0; //new ctrl sig for aui, luipc in Lab3
    end
    else begin
      ex_branch <= id_branch;
      ex_aluop <= id_aluop;
      ex_alusrc <= id_alusrc;
      ex_jump <= id_jump;
      ex_memread <= id_memread;
      ex_memwrite <= id_memwrite;
      ex_memtoreg <= id_memtoreg;
      ex_regwrite <= id_regwrite;
      ex_ui <= id_ui; //new ctrl sig for aui, luipc in Lab3
    end
  end
endmodule
