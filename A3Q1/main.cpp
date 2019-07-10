#include "systemc.h"
#include "printer.h"
#include "adder.h"
#include "gen.h"
#include "fork.h"
#include "hw_fifo.h"
#include "adapter.h"
#include <iostream>
#include <iomanip>


int sc_main (int argc, char* argv[]) {

  sc_clock clk ("Clock", 10, SC_NS, 0.5, 1, SC_NS);

  //DF_Adder <int> adder ("adder");
  DF_Const <int> constant ("constant", 1); // constant = 1
  DF_Fork <int> fork ("fork");
  DF_Printer <int> printer ("printer", 10); // 10 interations

  //sc_fifo <int> adder_out ("adder_out", 1); // FIFO of size 1
  sc_fifo <int> const_out ("const_out", 1); // FIFO of size 1
  sc_fifo <int> printer_in ("printer_in", 1); // FIFO of size 1
  sc_fifo <int> feedback ("feedback", 1); // FIFO of size 1

  HW_Adder <int> adder ("adder");
  FIFO_READ_HS <int> adapter_read ("Read_Adapter");
  hw_fifo <int> adder_out("adder_out",1);

  //Hardware Signals
  sc_signal <int> adder_data ("adder_data");
  sc_signal <bool> adder_valid ("adder_valid");
  sc_signal <bool> adder_ready ("adder_ready");
  sc_signal <int> fork_data ("fork_data");
  sc_signal <bool> adapter_valid ("adapter_valid");
  sc_signal <bool> adapter_ready ("adapter_ready");

  feedback.write (40); // do not forget about initialization!

  constant.output (const_out);
  printer.input (printer_in);

  //HW Adder
  adder.input1(const_out);
  adder.input2(feedback);
  adder.clock(clk);
  adder.data(adder_data);
  adder.valid(adder_valid);
  adder.ready(adder_ready);

  //HW FIFO
  adder_out.clk(clk);
  adder_out.data_in(adder_data);
  adder_out.valid_in(adder_valid);
  adder_out.ready_in(adapter_ready);

  adder_out.data_out(fork_data);
  adder_out.valid_out(adapter_valid);
  adder_out.ready_out(adder_ready);

  //Read adapter
  adapter_read.clock(clk);
  adapter_read.data(fork_data);
  adapter_read.valid(adapter_valid);
  adapter_read.ready(adapter_ready);

  //Fork
  fork.input(adapter_read);
  fork.output1(feedback);
  fork.output2(printer_in);

  // Start simulation without time limit
  // The printer module is responsible for stopping the simulation
  sc_start();
  return 0;
}
