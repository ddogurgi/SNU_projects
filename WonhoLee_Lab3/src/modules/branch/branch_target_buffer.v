// branch_target_buffer.v

/* The branch target buffer (BTB) stores the branch target address for
 * a branch PC. Our BTB is essentially a direct-mapped cache.
 */

module branch_target_buffer #(
  parameter DATA_WIDTH = 32,
  parameter NUM_ENTRIES = 256
) (
  input clk,
  input rstn,

  // update interface
  input update,                              // when 'update' is true, we update the BTB entry
  input [DATA_WIDTH-1:0] resolved_pc,
  input [DATA_WIDTH-1:0] resolved_pc_target,

  // access interface
  input [DATA_WIDTH-1:0] pc,

  output reg hit,
  output reg [DATA_WIDTH-1:0] target_address
);

// TODO: Implement BTB
reg [54:0] BTB [0:NUM_ENTRIES-1]; // Branch Target Buffer(valid, tag, pc target bits)
reg valid; //valid bit for pc address
reg [21:0] tag; // tag bits for pc address
reg [DATA_WIDTH-1:0] target; //target pc for pc address
integer i;
always @(posedge clk) begin
  if(~rstn)begin
    for(i = 0; i < NUM_ENTRIES; i = i + 1)begin
      BTB[i] = 55'b0; //reset BTB
    end
  end
end

always @(posedge clk) begin
  if(update)
    BTB[resolved_pc[9:2]] <= {1'b1, resolved_pc[31:10], resolved_pc_target}; //256 = 8-bit BTB idx
end

always @(*) begin
  {valid, tag, target} = BTB[pc[9:2]];
  if((valid == 1) && (tag == pc[31:10])) begin
    target_address = target;
    hit = 1;
  end
  else begin
    target_address = pc + 4;
    hit = 0;
  end
end

endmodule
