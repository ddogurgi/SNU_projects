// hardware_counter.v

module hardware_counter (
  input clk,
  input rstn,
  input cond,  // 1-bit cond signal

  output reg [31:0] counter // uint32 counter
);

  // increments the counter when 'cond' is high at the rising edge of the clock.
  always @(posedge clk) begin
    if (!rstn) begin
        counter <= 32'd0;
    end else begin
      if (cond) begin
        counter <= counter + 32'd1;
      end
    end   
  end

endmodule
