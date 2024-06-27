// imm_generator.v

module imm_generator #(
  parameter DATA_WIDTH = 32
)(
  input [31:0] instruction,

  output reg [DATA_WIDTH-1:0] sextimm
);

wire [6:0] opcode;
assign opcode = instruction[6:0];

always @(*) begin
  case (opcode)
    //////////////////////////////////////////////////////////////////////////
    // TODO : Generate sextimm using instruction
    //////////////////////////////////////////////////////////////////////////
    7'b0010011 : sextimm = {{21{instruction[31]}}, instruction[30:20]}; //I-type : arithmetic
    7'b0000011 : sextimm = instruction[14] ? {{20'b0, instruction[31:20]}}:{{21{instruction[31]}}, instruction[30:20]}; //I-type : load
    7'b0100011 : sextimm = {{21{instruction[31]}}, instruction[30:25], instruction[11:7]}; //S-type : Store
    7'b1100011 : sextimm = {{20{instruction[31]}}, instruction[7], instruction[30:25], instruction[11:8], 1'b0}; //SB-type : Conditional Jump
    7'b1101111 : sextimm = {{12{instruction[31]}}, instruction[19:12], instruction[20], instruction[30:21], 1'b0}; //UJ-type : Jump and Link
    7'b1100111 : sextimm = {{21{instruction[31]}}, instruction[30:20]}; //I-type : Jump and Link register
    7'b0110111 : sextimm = {instruction[31:12], 12'b0};//U-type : Load Upper immediate
    7'b0010111 : sextimm = {instruction[31:12], 12'b0};//U-type : Add Upper immediate to PC
    default:    sextimm = 32'h0000_0000;
  endcase
end


endmodule
