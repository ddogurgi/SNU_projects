// ALU_control.v

/* This unit generates a 4-bit ALU control input (alu_func)
 * based on the 2-bit ALUOp control, funct7, and funct3 field.
 *
 * ALUOp | ALU action | notes  
 * ------|------------|---------------------
 *   00  | add        | for loads and stores
 *   01  | subtract   | for branches
 *   10  | it varies  | for R-types
 *   11  | it varies  | immediate
 *
 * R-type instructions (opcode: 0110011)
 * Name | funct3 | funct7
 * -----------------------
 * add  |  0x0   | 0x00
 * sub  |  0x0   | 0x20
 * xor  |  0x4   | 0x00
 * or   |  0x6   | 0x00
 * and  |  0x7   | 0x00
 * sll  |  0x1   | 0x00
 * srl  |  0x5   | 0x00
 * sra  |  0x5   | 0x20
 * slt  |  0x2   | 0x00
 * sltu |  0x3   | 0x00
 */

`include "defines.v"

module ALU_control(
  input wire [1:0] alu_op,
  input wire [6:0] funct7,
  input wire [2:0] funct3,

  output reg [3:0] alu_func
);

wire [3:0] funct;
assign funct = {funct7[5], funct3};

// combinational logic
always @(*) begin
  case (alu_op)
    2'b00: begin
      ///////////////////////////////////////////////////////////////////////
      // TODO : select operation for loads/stores
      ///////////////////////////////////////////////////////////////////////
      alu_func = `OP_ADD;
    end
    2'b01: begin
      ///////////////////////////////////////////////////////////////////////
      // TODO : select operation for branches
      ///////////////////////////////////////////////////////////////////////
      case (funct3)
        3'b000: alu_func = `OP_SUB;
        3'b001: alu_func = `OP_BNE;
        3'b100: alu_func = `OP_BLT;
        3'b101: alu_func = `OP_BGE;
        3'b110: alu_func = `OP_BLTU;
        3'b111: alu_func = `OP_BGEU;
      endcase
    end
    2'b10: begin                // R-types
      case (funct)
        4'b0_000: alu_func = `OP_ADD;
        4'b1_000: alu_func = `OP_SUB;
        4'b0_100: alu_func = `OP_XOR;
        4'b0_110: alu_func = `OP_OR;
        4'b0_111: alu_func = `OP_AND;
        4'b0_001: alu_func = `OP_SLL;
        4'b0_101: alu_func = `OP_SRL;
        4'b1_101: alu_func = `OP_SRA;
        4'b0_010: alu_func = `OP_SLT;
        4'b0_011: alu_func = `OP_SLTU;
        default:  alu_func = `OP_EEE;  // should not fall here 
      endcase
    end
    2'b11: begin
      ///////////////////////////////////////////////////////////////////////
      // TODO : select operation for I-types with immediate
      ///////////////////////////////////////////////////////////////////////
      case (funct3)
        3'b000: alu_func = `OP_ADD;
        3'b100: alu_func = `OP_XOR;
        3'b110: alu_func = `OP_OR;
        3'b111: alu_func = `OP_AND;
        3'b001: alu_func = `OP_SLL;
        3'b101: alu_func = (funct7[5] == 0) ? `OP_SRL : `OP_SRA; // srai : imm[11:5] = 0x20
        3'b010: alu_func = `OP_SLT;
        3'b011: alu_func = `OP_SLTU;
        default:  alu_func = `OP_EEE;  // should not fall here 
      endcase
    end
    default: alu_func = `OP_EEE;       // should not fall here
  endcase
end

endmodule
