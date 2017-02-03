


#pragma once
#include <string>

#include <ssnlib_log.h>
#include <ssnlib_mempool.h>
#include <ssnlib_thread.h>



namespace ssnlib {



class Cpu_interface {
private:
    static int Exe(void* arg)
    {
        ssnlib::Cpu_interface* cpu = reinterpret_cast<ssnlib::Cpu_interface*>(arg);
        (*cpu->thread)();
        return 0;
    }

public:
	const uint8_t lcore_id;
    const std::string name;
    ssn_thread* thread;

	Cpu_interface(size_t lid) :
        lcore_id(lid),
        name("lcore" + std::to_string(lcore_id)),
        thread(nullptr)
    {
        if (lid >= rte_lcore_count()) {
            throw slankdev::exception("invalid lcore id");
        }

        kernel_log(SYSTEM, "boot  %s ... done\n", name.c_str());
    }
    ~Cpu_interface() { rte_eal_wait_lcore(lcore_id); }
	void launch()
	{
        if (thread) {
            if (lcore_id == 0) {
                fprintf(stderr, "This is COM core. can not launch thread");
            } else {
                kernel_log(SYSTEM, "%s lanching ... ", name.c_str());
                rte_eal_remote_launch(Cpu_interface::Exe, this, lcore_id);
                printf("done \n");
            }
        }
	}
    rte_lcore_state_t get_state()
    {
        return rte_eal_get_lcore_state(lcore_id);
    }
};



} /* namespace ssnlib */

