
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <slankdev/exception.h>
#include <slankdev/signal.h>

class Port {
 public:
  size_t i_rxp = 0;
  size_t o_rxp = 0;
  size_t get_inner_rx_perf() const { return i_rxp; }
  size_t get_outer_rx_perf() const { return o_rxp; }
  double get_perf_reduct() const
  {
    size_t val_i = get_inner_rx_perf();
    size_t val_o = get_outer_rx_perf();
    if (val_o == 0) return 1;
    double ret = double(val_i)/double(val_o);
    return ret;
  }
  void debug_dump(FILE* fp) const
  {
    size_t irx = get_inner_rx_perf();
    size_t orx = get_outer_rx_perf();
    double perf_reduct = get_perf_reduct();
    fprintf(fp, "  irx: %zd \n", irx);
    fprintf(fp, "  orx: %zd \n", orx);
    fprintf(fp, "  red: %lf \n", perf_reduct);
  }
};

class Vnf {
  double reduct_threshold = 0.7;
  Port ports[2];
 public:
  size_t n_lcores;
  Vnf() : n_lcores(1) {}
  void set_load_high()
  {
    ports[0].i_rxp = 2*n_lcores;
    ports[1].i_rxp = 2*n_lcores;
    ports[0].o_rxp = 10;
    ports[1].o_rxp = 10;
  }
  void set_load_low()
  {
    ports[0].i_rxp = 0;
    ports[0].o_rxp = 0;
    ports[1].i_rxp = 0;
    ports[1].o_rxp = 0;
  }
  void perf_switch()
  {
    static bool condition = true;
    condition = condition?false:true;
    if (condition) set_load_low();
    else           set_load_high();
  }
  double perf_red() const
  {
    double port0_red = ports[0].get_perf_reduct();
    double port1_red = ports[1].get_perf_reduct();
    size_t n_ports = 2;
    double sum_red = (port0_red + port1_red)/n_ports;
    return sum_red;
  }
  void debug_dump(FILE* fp) const
  {
    auto sum_red = perf_red();
    fprintf(fp, "vnf: red=%.2lf n_cores=%zd\n", sum_red, n_lcores);
  }
  bool is_need_powerup() const { return perf_red()<reduct_threshold; }
} vnf;

bool d2engine_running = true;
void d2engine()
{
  while (d2engine_running) {
    if (vnf.is_need_powerup()) {
      printf("POWERUP!!\n");
    }
    vnf.debug_dump(stdout);
    sleep(1);
  }
}

bool control_running = true;
void control()
{
  while (control_running) {
    getchar();
    vnf.set_load_high();
    vnf.n_lcores ++;
  }
}

void signal_handler(int signum)
{
  d2engine_running = false;
  control_running = false;
}

int main(int argc, char** argv)
{
  slankdev::signal(SIGINT, signal_handler);
  std::thread t0(d2engine);
  std::thread t1(control);
  t0.join();
  t1.join();
  printf("finish \n");
}


