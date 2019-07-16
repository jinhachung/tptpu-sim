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
	Interconnect *cpu_ub_icnt = new Interconnect((Unit *)cpu, (Unit *)ub, (float)1, (float)1000, ub->GetCapacity(),
												 cpu->IsMainMemory(), cpu->GetSenderQueue(),
												 ub->GetServedQueue(), ub->GetWaitingQueue(), ub->GetRequestQueue());
	// setting complete

	request req1 = MakeRequest(1, 3000);
	request req2 = MakeRequest(2, 6000);
	request req3 = MakeRequest(3, 9000);
	request req4 = MakeRequest(4, 12000);
	request req5 = MakeRequest(5, 15000);

	cpu_ub_icnt->ReceiveRequest(req1);
	cpu_ub_icnt->ReceiveRequest(req2);
	cpu_ub_icnt->ReceiveRequest(req3);
	cpu_ub_icnt->ReceiveRequest(req4);
	cpu_ub_icnt->ReceiveRequest(req5);

	while (!cpu_ub_icnt->IsIdle()) {
		ub->Cycle();
		cpu_ub_icnt->Cycle();
		cpu->Cycle();
	}

	// test complete
	cpu_ub_icnt->PrintStats("CPU - Unified Buffer Interconnect");
	return 0;
}
