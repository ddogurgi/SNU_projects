// hazard.v

// This module determines if pipeline stalls or flushing are required

// TODO: declare propoer input and output ports and implement the
// hazard detection unit

module hazard (
    input [1:0] mem_jump,
    input mem_taken,
    input mem_branch,
    input [31:0] mem_pc_target, //check branch prediction - actual pc
    input [31:0] mem_branch_predict, //check branch prediction - predicted pc
    input [6:0] if_opcode,
    input hit,
    input pred,
    input [4:0] ex_rd,
    input [4:0] id_rs1,
    input [4:0] id_rs2,
    input ex_memread,
    

    output reg flush,
    output reg stall,
    output reg [1:0] PC_src,
    output reg mem_false_taken
);
reg if_jump;
reg if_branch;
always@(*) begin
    if(if_opcode == 7'b1100011) begin
        if_branch <= 1;
        if_jump <= 0;
    end
    else if((if_opcode == 7'b1101111) || (if_opcode == 7'b1100111))begin
        if_branch <= 0;
        if_jump <= 1;
    end
    else begin
        if_branch <= 0;
        if_jump <= 0;
    end
end
always @(*) begin
    //recovery from misprediction
    //predicted not taken but actually taken 
    if((mem_jump[1] | mem_taken) && (mem_pc_target != mem_branch_predict)) begin
        flush <= 1'b1;
        stall <= 1'b0;
        PC_src <= 2'b01;
        mem_false_taken <= 1'b0;
    end
    //predicted taken but actually not taken
    else if((mem_branch && (~mem_taken)) &&(mem_pc_target == mem_branch_predict)) begin
        flush <= 1'b1;
        stall <= 1'b0;
        PC_src <= 2'b01;
        mem_false_taken <= 1'b1;
    end
    //pc stall for load instruction hazard
    else if(((ex_rd == id_rs1) || (ex_rd == id_rs2)) && (ex_memread)) begin
        flush <= 1'b0;
        stall <= 1'b1;
        PC_src <= 2'b10;
        mem_false_taken <= 1'b0;
    end
    //branch prediction
    else if((if_jump |(if_branch & pred)) & hit) begin
        flush <= 1'b0;
        stall <= 1'b0;
        PC_src <= 2'b11;
        mem_false_taken <= 1'b0;
    end
    //normal : pc = pc + 4
    else begin
        flush <= 1'b0;
        stall <= 1'b0;
        PC_src <= 2'b00;
        mem_false_taken <= 1'b0;
    end
end

endmodule
