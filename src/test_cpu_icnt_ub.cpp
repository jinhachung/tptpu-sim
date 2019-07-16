#include <iostream>
#include <cstdlib>

#include "buffer.hpp"
#include "common.hpp"
#include "cpu.hpp"
#include "interconnect.hpp"

int main(int argc, char *argv[]) {
	CPU *cpu = new CPU();
	// 30KB per buffer, 60KB total
	UnifiedBuffer *ub = new UnifiedBuffer(30000);
	// 1 GHz, 1000 GB/s
	Interconnect *cpu_ub_icnt = new Interconnect((void *)cpu, (void *)ub, (float)1, (float)1000, ub->GetCapacity(),
												 cpu->IsMainMemory(), cpu->GetSenderQueue(),
												 ub->GetServedQueue(), ub->GetWaitingQueue(), ub->GetRequestQueue());
	// setting complete

	request req10_1 = MakeRequest(1, 10000);
	request req10_2 = MakeRequest(2, 10000);
	request req15_3 = MakeRequest(3, 15000);
	request req15_4 = MakeRequest(4, 15000);
	request req20_5 = MakeRequest(5, 20000);
	request req20_6 = MakeRequest(6, 20000);

	cpu_ub_icnt->ReceiveRequest(req10_1);
	cpu_ub_icnt->ReceiveRequest(req10_2);
	cpu_ub_icnt->ReceiveRequest(req15_3);
	cpu_ub_icnt->ReceiveRequest(req15_4);
	cpu_ub_icnt->ReceiveRequest(req20_5);
	cpu_ub_icnt->ReceiveRequest(req20_6);

	while (!cpu_ub_icnt->IsIdle()) {
		ub->Cycle();
		cpu_ub_icnt->Cycle();
		cpu->Cycle();
	}

	// test complete
	cpu_ub_icnt->PrintStats("CPU - Unified Buffer Interconnect");
	return 0;
}
