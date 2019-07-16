#include <iostream>
#include <cstdlib>

#include "buffer.hpp"
#include "common.hpp"
#include "cpu.hpp"
#include "dram.hpp"
#include "interconnect.hpp"
#include "unit.hpp"
#include "weightfetcher.hpp"

int main(int argc, char *argv[]) {
	CPU *cpu = new CPU();
	// 30KB per buffer, 60KB total
	UnifiedBuffer *ub = new UnifiedBuffer(30000);
	DRAM *dram = new DRAM();
	// 64KiB per tile, 4 tiles deep
	WeightFetcher *wf = new WeightFetcher(65536, 4);
	float clock = 1;	// 1GHz
	float bw = 1000;	// 1000GB/s
	Interconnect *cpu_ub_icnt = new Interconnect((Unit *)cpu, (Unit *)ub, clock, bw, ub->GetCapacity(),
												 cpu->IsMainMemory(), cpu->GetSenderQueue(),
												 ub->GetServedQueue(), ub->GetWaitingQueue(), ub->GetRequestQueue());
	Interconnect *dram_wf_icnt = new Interconnect((Unit *)dram, (Unit *)wf, clock, bw, wf->GetCapacity(),
												  dram->IsMainMemory(), dram->GetSenderQueue(),
												  wf->GetServedQueue(), wf->GetWaitingQueue(), wf->GetRequestQueue());
	// setting complete
	int id = 1;
	
	for (int i = 0; i < 6; i++) {
		cpu_ub_icnt->ReceiveRequest(MakeRequest(id, 1000 * (2 * i + 1)));
		id++;
	}
	
	for (int j = 0; j < 4; j++) {
		dram_wf_icnt->ReceiveRequest(MakeRequest(id, 65536));
		id++;
	}

	while (!cpu_ub_icnt->IsIdle() || !dram_wf_icnt->IsIdle()) {
		ub->Cycle();
		cpu_ub_icnt->Cycle();
		cpu->Cycle();

		wf->Cycle();
		dram_wf_icnt->Cycle();
		dram->Cycle();
	}

	// test complete
	cpu_ub_icnt->PrintStats("CPU - Unified Buffer Interconnect");
	dram_wf_icnt->PrintStats("DRAM - Weight Fetcher Interconnect");
	return 0;
}
