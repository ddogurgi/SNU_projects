// ifid_reg.v
// This module is the IF/ID pipeline register.


module ifid_reg #(
  parameter DATA_WIDTH = 32
)(
  // TODO: Add flush or stall signal if it is needed
  input flush,
  input stall,
  //////////////////////////////////////
  // Inputs
  //////////////////////////////////////
  input clk,

  input [DATA_WIDTH-1:0] if_PC,
  input [DATA_WIDTH-1:0] if_pc_plus_4,
  input [DATA_WIDTH-1:0] if_instruction,
  input [DATA_WIDTH-1:0] if_branch_predict,
  //////////////////////////////////////
  // Outputs
  //////////////////////////////////////
  output reg [DATA_WIDTH-1:0] id_PC,
  output reg [DATA_WIDTH-1:0] id_pc_plus_4,
  output reg [DATA_WIDTH-1:0] id_instruction,
  output reg [DATA_WIDTH-1:0] id_branch_predict
);

// TODO: Implement IF/ID pipeline register module
  always @(posedge clk) begin
    if(flush == 1) begin
      id_PC <= if_PC;
      id_pc_plus_4 <= if_pc_plus_4;
      id_instruction <= 32'b0;
      id_branch_predict <= if_branch_predict;
    end
    else if(stall == 1) begin
      id_PC <= id_PC;
      id_pc_plus_4 <= id_pc_plus_4;
      id_instruction <= id_instruction;
      id_branch_predict <= id_branch_predict;
    end
    else begin
      id_PC <= if_PC;
      id_pc_plus_4 <= if_pc_plus_4;
      id_instruction <= if_instruction;
      id_branch_predict <= if_branch_predict;
    end
  end
endmodule
