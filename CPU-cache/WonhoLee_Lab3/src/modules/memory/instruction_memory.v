// instruction_memory.v

module instruction_memory #(
  parameter DATA_WIDTH = 32, MEM_ADDR_SIZE = 9
)(
  input [DATA_WIDTH-1:0] address,

  output reg [DATA_WIDTH-1:0] instruction
);

  // memory
  reg [DATA_WIDTH-1:0] inst_memory[0:2**MEM_ADDR_SIZE-1];
  initial $readmemb("data/inst.mem", inst_memory);
  
  // read instruction
  always @(*) begin
    instruction = inst_memory[address[MEM_ADDR_SIZE+1:2]];
  end

endmodule
