// gshare.v

/* The Gshare predictor consists of the global branch history register (BHR)
 * and a pattern history table (PHT). Note that PC[1:0] is not used for
 * indexing.
 */

module gshare #(
  parameter DATA_WIDTH = 32,
  parameter COUNTER_WIDTH = 2,
  parameter NUM_ENTRIES = 256
) (
  input clk,
  input rstn,

  // update interface
  input update,
  input actually_taken,
  input [DATA_WIDTH-1:0] resolved_pc,

  // access interface
  input [DATA_WIDTH-1:0] pc,

  output reg pred
);

  // TODO: Implement gshare branch predictor
reg [1:0] PHT [0:NUM_ENTRIES-1]; //Pattern history table
reg [7:0] BHR; //Global Branch history register
reg [7:0] resvpc;
always @(posedge clk) begin
  if(~rstn) begin
    for(integer i = 0; i < NUM_ENTRIES; i = i + 1) begin
      PHT[i] = 2'b01; //reset PHT(weaky NT 01)
    end
    BHR = 8'b0; //reset BHR(all 0)
  end
end

always @(posedge clk) begin
  if(update)begin
    resvpc = resolved_pc[9:2];
    if(actually_taken) begin
      if(PHT[BHR ^ resvpc] != 2'b11)
        PHT[BHR ^ resvpc] = PHT[BHR ^ resvpc] + 1;
    end
    else begin
      if(PHT[BHR ^ resvpc] != 2'b00)
        PHT[BHR ^ resvpc] = PHT[BHR ^ resvpc] - 1;
    end
    BHR = {BHR[6:0], actually_taken};
    
  end
end

always @(*) begin
  if(PHT[BHR ^ pc[9:2]] >= 2'b10)
    pred = 1;
  else
    pred = 0;
end

endmodule
