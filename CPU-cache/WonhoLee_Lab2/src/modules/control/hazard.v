// hazard.v

// This module determines if pipeline stalls or flushing are required

// TODO: declare propoer input and output ports and implement the
// hazard detection unit

module hazard (
    input [1:0] ex_jump,
    input ex_taken,

    input [4:0] ex_rd,
    input [4:0] id_rs1,
    input [4:0] id_rs2,
    input ex_memread,
    
    output reg flush,
    output reg stall,
    output reg [1:0] PC_src
);

always @(*) begin
    if(ex_jump[1] | ex_taken != 0) begin
        flush <= 1'b1;
        stall <= 1'b0;
        PC_src <= 2'b01;
    end
    else if(((ex_rd == id_rs1) || (ex_rd == id_rs2)) && (ex_memread)) begin
        flush <= 1'b0;
        stall <= 1'b1;
        PC_src <= 2'b10;
    end
    else begin
        flush <= 1'b0;
        stall <= 1'b0;
        PC_src <= 2'b00;
    end
end

endmodule
