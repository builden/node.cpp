// g3log-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <nodecpp/g3log.h>

namespace example_fatal
{
  void killWithContractIfNonEqual(int first, int second)
  {
    CHECK(first == second) << "Test to see if contract works: onetwothree: " << 123 << ". This should be at the end of the log, and will exit this example";
  }
} // example fatal

int _tmain(int argc, _TCHAR* argv[])
{
  double pi_d = 3.1415926535897932384626433832795;
  float pi_f = 3.1415926535897932384626433832795f;

  g3log.init("log", "./logs");
  
  std::cout << "*   This is an example of g3log. It WILL exit by a failed CHECK(...)" << std::endl;
  std::cout << "*   that acts as a FATAL trigger. Please see the generated log and " << std::endl;
  std::cout << "*   compare to the code at:\n*  \t g3log/test_example/main_contract.cpp" << std::endl;
  // std::cout << "*\n*   Log file: [" << log_file_name.get() << "]\n\n" << std::endl;

  LOGF(INFO, "Hi log %d", 123);
  LOG(INFO) << "Test SLOG INFO";
  LOG(DEBUG) << "Test SLOG DEBUG";
  LOG(INFO) << "one: " << 1;
  LOG(INFO) << "two: " << 2;
  LOG(INFO) << "one and two: " << 1 << " and " << 2;
  LOG(DEBUG) << "float 2.14: " << 1000 / 2.14f;
  LOG(DEBUG) << "pi double: " << pi_d;
  LOG(DEBUG) << "pi float: " << pi_f;
  LOG(DEBUG) << "pi float (width 10): " << std::setprecision(10) << pi_f;
  LOGF(INFO, "pi float printf:%f", pi_f);

  // FATAL SECTION
  int smaller = 1;
  int larger = 2;
  example_fatal::killWithContractIfNonEqual(smaller, larger);
  g3log.finalize();

	return 0;
}

