`timescale 1 ns/10 ps

module riscv_tb;

reg clk, rstn;
wire [31:0] inst;

// find the number of instructions in inst.mem
// NOTE: each line in inst.mem must be 32-bit PC + \n
//       otherwise, simulation can be wrong 
// NOTE: <end> of the inst.mem consists of NOPs & JAL to NOPs
integer fd, position, n_instructions;
reg [31:0] LAST_PC;
initial begin
  fd = $fopen("data/inst.mem", "r");
  position = $fseek(fd, 0, 2);  // SEEK_END: 2
  position = $ftell(fd);
  $fclose(fd);

  n_instructions = position / (32 + 1);  // 32-bit instr + \n

  LAST_PC = (n_instructions - 1) * 4;

  $display("File has %0d instrs", n_instructions);
  $display("Last PC is %0d", LAST_PC);
end

integer i;

initial begin
  clk  = 1'b0;
  rstn = 1'b0;
  $display($time, " ** Start Simulation **");
  $display($time, " Instruction Memory ");
  $monitor($time, " [PC] pc : %d", my_cpu.PC);
  #60 rstn = 1'b1;
  //#4000
  wait (my_cpu.PC == LAST_PC);  
  rstn = 1'b0;
  $display($time, " ** End Simulation **");
  
  ////////////////////////////////////////////////////////
  // [WARNING] : DO NOT ERASE when using "test.py"
  ////////////////////////////////////////////////////////
  $display($time, " REGISTER FILE");
  for (i=0;i<32;i=i+1) $display($time, " Reg[%d]: %d (%b)", i, $signed(my_cpu.m_register_file.reg_array[i]), my_cpu.m_register_file.reg_array[i]);
  //$display($time, " DATA MEMORY");
  //for (i=0;i<128;i=i+1) $display($time, " Mem[%d]: %d (%b)", i, $signed(my_cpu.m_data_memory.mem_array[i]), my_cpu.m_data_memory.mem_array[i]);

  
  $display($time, " HARDWARE COUNTERS");
  $display($time, " CORE_CYCLE: %d", my_cpu.CORE_CYCLE);

  $display($time, " NUM_COND_BRANCHES: %d", my_cpu.NUM_COND_BRANCHES);
  $display($time, " NUM_UNCOND_BRANCHES: %d", my_cpu.NUM_UNCOND_BRANCHES);

  $display($time, " BP_CORRECT: %d", my_cpu.BP_CORRECT);
  $display($time, " BP_INCORRECT: %d", my_cpu.NUM_COND_BRANCHES - my_cpu.BP_CORRECT);

  $finish;
end

always begin
  #5 clk = ~clk;
end

// dump the state of the design
// VCD (Value Change Dump) is a standard dump format defined in Verilog.
initial begin
  $dumpfile("sim.vcd");
  $dumpvars(0, riscv_tb);
end

simple_cpu my_cpu(.clk(clk), .rstn(rstn));

endmodule
