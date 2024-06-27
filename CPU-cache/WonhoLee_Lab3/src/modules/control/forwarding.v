// forwarding.v

// This module determines if the values need to be forwarded to the EX stage.

// TODO: declare propoer input and output ports and implement the
// forwarding unit

module forwarding (
    input [4:0] idex_rs1,
    input [4:0] idex_rs2,
    input [4:0] exmem_rd,
    input [4:0] memwb_rd,
    input mem_regwrite,
    input wb_regwrite,

    output reg [1:0] forwardA,
    output reg [1:0] forwardB
);

always @(*) begin
    if((idex_rs1 != 5'b0) && (idex_rs1 == exmem_rd) && (mem_regwrite))
        forwardA = 2'b10;
    else if((idex_rs1 != 5'b0) && (idex_rs1 == memwb_rd) && (wb_regwrite))
        forwardA = 2'b01;
    else
        forwardA = 2'b00;
    if((idex_rs2 != 5'b0) && (idex_rs2 == exmem_rd) && (mem_regwrite))
        forwardB = 2'b10;
    else if((idex_rs2 != 5'b0) && (idex_rs2 == memwb_rd) && (wb_regwrite))
        forwardB = 2'b01;
    else
        forwardB = 2'b00;
end

endmodule
