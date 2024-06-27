// simple_cpu.v
// a pipelined RISC-V microarchitecture (RV32I)

///////////////////////////////////////////////////////////////////////////////////////////
//// [*] In simple_cpu.v you should connect the correct wires to the correct ports
////     - All modules are given so there is no need to make new modules
////       (it does not mean you do not need to instantiate new modules)
////     - However, you may have to fix or add in / out ports for some modules
////     - In addition, you are still free to instantiate simple modules like multiplexers,
////       adders, etc.
///////////////////////////////////////////////////////////////////////////////////////////

module simple_cpu
#(parameter DATA_WIDTH = 32)(
  input clk,
  input rstn
);

///////////////////////////////////////////////////////////////////////////////
// TODO:  Declare all wires / registers that are needed
///////////////////////////////////////////////////////////////////////////////
// e.g., wire [DATA_WIDTH-1:0] if_pc_plus_4;
// 1) Pipeline registers (wires to / from pipeline register modules)
// 2) In / Out ports for other modules
// 3) Additional wires for multiplexers or other mdoules you instantiate

reg [DATA_WIDTH-1:0] PC;    // program counter (32 bits)
wire [DATA_WIDTH-1:0] PC_PLUS_4;
wire [DATA_WIDTH-1:0] NEXT_PC;
wire [DATA_WIDTH-1:0] id_pc_plus_4;
wire [DATA_WIDTH-1:0] id_PC;
wire [DATA_WIDTH-1:0] id_instruction;
wire [1:0] id_jump;
wire id_branch;
wire [1:0] id_aluop;
wire id_alusrc;
wire id_memread;
wire id_memwrite;
wire id_memtoreg;
wire id_regwrite;
wire [DATA_WIDTH-1:0] id_sextimm, id_readdata1, id_readdata2;
wire [DATA_WIDTH-1:0] id_alu_out;
wire [4:0] id_rs1, id_rs2, id_rd;
wire [6:0] id_opcode;
wire [6:0] id_funct7;
wire [2:0] id_funct3;
wire flush;
wire stall;
wire [1:0] PC_src;
wire [DATA_WIDTH-1:0] ex_PC;
wire [DATA_WIDTH-1:0] ex_pc_plus_4, ex_pc_target;
wire ex_branch;
wire [1:0] ex_aluop;
wire ex_alusrc;
wire [1:0] ex_jump;
wire ex_memread;
wire ex_memwrite;
wire ex_memtoreg;
wire ex_regwrite;
wire [DATA_WIDTH-1:0] ex_sextimm, ex_readdata1, ex_readdata2, ex_alu_result;
wire [4:0] ex_rs1, ex_rs2, ex_rd;
wire [6:0] ex_funct7;
wire [2:0] ex_funct3;
wire [DATA_WIDTH-1:0] ex_in_b;
wire ex_taken;
wire [DATA_WIDTH-1:0] ex_adds;
wire ex_alu_check;
wire [3:0] ex_alu_func;
wire [1:0] forwardA, forwardB;
wire [DATA_WIDTH-1:0] ex_forwardedA; 
wire [DATA_WIDTH-1:0] ex_forwardedB;
wire [DATA_WIDTH-1:0] mem_readdata;
wire [DATA_WIDTH-1:0] mem_pc_plus_4, mem_pc_target;
wire mem_taken, mem_memread, mem_memwrite, mem_memtoreg, mem_regwrite;
wire [1:0] mem_jump;
wire [DATA_WIDTH-1:0] mem_alu_result;
wire [DATA_WIDTH-1:0] mem_writedata;
wire [2:0] mem_funct3;
wire [4:0] mem_rd;
wire [DATA_WIDTH - 1:0] wb_pc_plus_4, wb_readdata, wb_alu_result;
wire [4:0] wb_rd;
wire [1:0] wb_jump;
wire wb_memtoreg, wb_regwrite;
wire [DATA_WIDTH-1:0] write_data;

///////////////////////////////////////////////////////////////////////////////
// Instruction Fetch (IF)
///////////////////////////////////////////////////////////////////////////////

/* m_next_pc_adder */
adder m_pc_plus_4_adder(
  .in_a   (PC),
  .in_b   (32'h0000_0004),

  .result (PC_PLUS_4)
);

always @(posedge clk) begin
  if (rstn == 1'b0) begin
    PC <= 32'h00000000;
  end
  else PC <= NEXT_PC;
end
mux_3x1 m_next_pc(
  .select     (PC_src),
  .in1        (PC_PLUS_4),
  .in2        (ex_pc_target),
  .in3        (PC),

  .out        (NEXT_PC)
);
/* instruction: read current instruction from inst mem */
wire [DATA_WIDTH-1:0] instruction;
instruction_memory m_instruction_memory(
  .address    (PC),

  .instruction(instruction)
);

/* forward to IF/ID stage registers */
ifid_reg m_ifid_reg(
  // TODO: Add flush or stall signal if it is needed
  .flush          (flush),
  .stall          (stall),
  .clk            (clk),
  .if_PC          (PC),
  .if_pc_plus_4   (PC_PLUS_4),
  .if_instruction (instruction),

  .id_PC          (id_PC),
  .id_pc_plus_4   (id_pc_plus_4),
  .id_instruction (id_instruction)
);

//////////////////////////////////////////////////////////////////////////////////
// Instruction Decode (ID)
//////////////////////////////////////////////////////////////////////////////////

assign id_opcode = id_instruction[6:0];
assign id_funct7 = id_instruction[31:25];
assign id_funct3 = id_instruction[14:12];
assign id_rs1 = id_instruction[19:15];
assign id_rs2 = id_instruction[24:20];
assign id_rd  = id_instruction[11:7];

/* m_hazard: hazard detection unit */
hazard m_hazard(
  // TODO: implement hazard detection unit & do wiring
  .ex_jump(ex_jump),
  .ex_taken(ex_taken),
  .ex_rd(ex_rd),
  .id_rs1(id_rs1),
  .id_rs2(id_rs2),
  .ex_memread(ex_memread),

  .flush(flush),
  .stall(stall),
  .PC_src(PC_src)
);

/* m_control: control unit */
control m_control(
  .opcode     (id_opcode),

  .jump       (id_jump),
  .branch     (id_branch),
  .alu_op     (id_aluop),
  .alu_src    (id_alusrc),
  .mem_read   (id_memread),
  .mem_to_reg (id_memtoreg),
  .mem_write  (id_memwrite),
  .reg_write  (id_regwrite)
);

/* m_imm_generator: immediate generator */
immediate_generator m_immediate_generator(
  .instruction(id_instruction),

  .sextimm    (id_sextimm)
);

/* m_register_file: register file */
register_file m_register_file(
  .clk        (clk),
  .readreg1   (id_rs1),
  .readreg2   (id_rs2),
  .writereg   (wb_rd),
  .wen        (wb_regwrite),
  .writedata  (write_data),

  .readdata1  (id_readdata1),
  .readdata2  (id_readdata2)
);

/* forward to ID/EX stage registers */
idex_reg m_idex_reg(
  // TODO: Add flush or stall signal if it is needed
  .flush        (flush),
  .stall        (stall),
  .clk          (clk),
  .id_PC        (id_PC),
  .id_pc_plus_4 (id_pc_plus_4),
  .id_jump      (id_jump),
  .id_branch    (id_branch),
  .id_aluop     (id_aluop),
  .id_alusrc    (id_alusrc),
  .id_memread   (id_memread),
  .id_memwrite  (id_memwrite),
  .id_memtoreg  (id_memtoreg),
  .id_regwrite  (id_regwrite),
  .id_sextimm   (id_sextimm),
  .id_funct7    (id_funct7),
  .id_funct3    (id_funct3),
  .id_readdata1 (id_readdata1),
  .id_readdata2 (id_readdata2),
  .id_rs1       (id_rs1),
  .id_rs2       (id_rs2),
  .id_rd        (id_rd),

  .ex_PC        (ex_PC),
  .ex_pc_plus_4 (ex_pc_plus_4),
  .ex_jump      (ex_jump),
  .ex_branch    (ex_branch),
  .ex_aluop     (ex_aluop),
  .ex_alusrc    (ex_alusrc),
  .ex_memread   (ex_memread),
  .ex_memwrite  (ex_memwrite),
  .ex_memtoreg  (ex_memtoreg),
  .ex_regwrite  (ex_regwrite),
  .ex_sextimm   (ex_sextimm),
  .ex_funct7    (ex_funct7),
  .ex_funct3    (ex_funct3),
  .ex_readdata1 (ex_readdata1),
  .ex_readdata2 (ex_readdata2),
  .ex_rs1       (ex_rs1),
  .ex_rs2       (ex_rs2),
  .ex_rd        (ex_rd)
);

//////////////////////////////////////////////////////////////////////////////////
// Execute (EX) 
//////////////////////////////////////////////////////////////////////////////////

mux_2x1 m_rs2orimm(
  .select (ex_alusrc),
  .in1 (ex_forwardedB),
  .in2 (ex_sextimm),

  .out (ex_in_b)
);

/* m_branch_target_adder: PC + imm for branch address */
adder m_branch_target_adder(
  .in_a     (ex_PC),
  .in_b     (ex_sextimm),

  .result   (ex_adds)
);

assign ex_pc_target = (ex_jump[0]) ? ex_alu_result : ex_adds;

/* alu control : generates alu_func signal */
alu_control m_alu_control(
  .alu_op   (ex_aluop),
  .funct7   (ex_funct7),
  .funct3   (ex_funct3),

  .alu_func (ex_alu_func)
);


/* m_alu */
alu m_alu(
  .alu_func (ex_alu_func),
  .in_a     (ex_forwardedA), 
  .in_b     (ex_in_b), 

  .result   (ex_alu_result),
  .check    (ex_alu_check)
);

/* m_branch_control : checks T/NT */
branch_control m_branch_control(
  .branch (ex_branch),
  .check  (ex_alu_check),
  
  .taken  (ex_taken)
);



forwarding m_forwarding(
  // TODO: implement forwarding unit & do wiring
  .idex_rs1       (ex_rs1),
  .idex_rs2       (ex_rs2),
  .exmem_rd       (mem_rd),
  .memwb_rd       (wb_rd),
  .mem_regwrite   (mem_regwrite),
  .wb_regwrite    (wb_regwrite),

  .forwardA       (forwardA),
  .forwardB       (forwardB)
);

mux_3x1 forwardingA(
  .select         (forwardA),
  .in1            (ex_readdata1),
  .in2            (write_data),
  .in3            (mem_alu_result),

  .out            (ex_forwardedA)
);

mux_3x1 forwardingB(
  .select         (forwardB),
  .in1            (ex_readdata2),
  .in2            (write_data),
  .in3            (mem_alu_result),

  .out            (ex_forwardedB)
);

/* forward to EX/MEM stage registers */
exmem_reg m_exmem_reg(
  // TODO: Add flush or stall signal if it is needed
  .clk            (clk),
  .ex_pc_plus_4   (ex_pc_plus_4),
  .ex_pc_target   (ex_pc_target),
  .ex_taken       (ex_taken), 
  .ex_jump        (ex_jump),
  .ex_memread     (ex_memread),
  .ex_memwrite    (ex_memwrite),
  .ex_memtoreg    (ex_memtoreg),
  .ex_regwrite    (ex_regwrite),
  .ex_alu_result  (ex_alu_result),
  .ex_writedata   (ex_forwardedB),
  .ex_funct3      (ex_funct3),
  .ex_rd          (ex_rd),
  
  .mem_pc_plus_4  (mem_pc_plus_4),
  .mem_pc_target  (mem_pc_target),
  .mem_taken      (mem_taken), 
  .mem_jump       (mem_jump),
  .mem_memread    (mem_memread),
  .mem_memwrite   (mem_memwrite),
  .mem_memtoreg   (mem_memtoreg),
  .mem_regwrite   (mem_regwrite),
  .mem_alu_result (mem_alu_result),
  .mem_writedata  (mem_writedata),
  .mem_funct3     (mem_funct3),
  .mem_rd         (mem_rd)
);


//////////////////////////////////////////////////////////////////////////////////
// Memory (MEM) 
//////////////////////////////////////////////////////////////////////////////////

/* m_data_memory : main memory module */
data_memory m_data_memory(
  .clk         (clk),
  .address     (mem_alu_result),
  .write_data  (mem_writedata),
  .mem_read    (mem_memread),
  .mem_write   (mem_memwrite),
  .maskmode    (mem_funct3[1:0]),
  .sext        (mem_funct3[2]),

  .read_data   (mem_readdata)
);

/* forward to MEM/WB stage registers */
memwb_reg m_memwb_reg(
  // TODO: Add flush or stall signal if it is needed
  .clk            (clk),
  .mem_pc_plus_4  (mem_pc_plus_4),
  .mem_jump       (mem_jump),
  .mem_memtoreg   (mem_memtoreg),
  .mem_regwrite   (mem_regwrite),
  .mem_readdata   (mem_readdata),
  .mem_alu_result (mem_alu_result),
  .mem_rd         (mem_rd),

  .wb_pc_plus_4   (wb_pc_plus_4),
  .wb_jump        (wb_jump),
  .wb_memtoreg    (wb_memtoreg),
  .wb_regwrite    (wb_regwrite),
  .wb_readdata    (wb_readdata),
  .wb_alu_result  (wb_alu_result),
  .wb_rd          (wb_rd)
);

//////////////////////////////////////////////////////////////////////////////////
// Write Back (WB) 
//////////////////////////////////////////////////////////////////////////////////

mux_3x1 m_writedata (
  .select ({wb_jump[1], wb_memtoreg}),
  .in1    (wb_alu_result),
  .in2    (wb_readdata),
  .in3    (wb_pc_plus_4),
  .out    (write_data)
);

endmodule