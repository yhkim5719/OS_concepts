#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// intermediate form of input file
std::vector<int> input_file;

// number of processes
int num_p;

// process struct
struct proc {			
	int pid;
	int arrival_time;
	int exe_time;		
	int exit_time;			// exit from the memory
	int mem_pieces;
	std::vector<int> mem_size;
	int total_mem;
};

// all processes
std::vector<proc> procs;	

// event timeline, process arrival, process completion
std::vector<int> timeline;

// page struct
struct page {
	int range_s;
	int range_e;
	int pid;
	int page_num;
};

// whole memory struct
struct memory {
	int capacity;
	int rest_cap;
	int page_size;
	std::vector<page> pages;
} mem;

std::vector<int> p_que;
double turnaround_time = 0.0;

int main (int argc, char** argv) {
	
	//read file, argc 2(filename.exe inputfile.txt);
	if (argc != 2) {
		std::cout << "Usage: ./MM input_file.txt" << std::endl;
		return 0;
	}

	std::fstream file;
	std::string word;

	file.open(argv[1]);
	if (!file.is_open()) {
		std::cout << "Cannot open " << argv[1] << std::endl;
		return 0;
	}
	while (file >> word) {
		 input_file.push_back(stoi(word));
	}

	//input memory size, select page size(option: 1,2,3);
	
	std::cout << "Enter Memory Size: ";
	getline (std::cin, word);
	mem.capacity = stoi(word);
	if (mem.capacity > 30000) {
		std::cout << "The maximum Memory Size is 30,000." << '\n';
		return 0;
	}
	std::cout << "Select a Page Size (1: 100, 2: 200, 3: 400): ";
	getline (std::cin, word);
	std::cout << "selected " << word << '\n';
	switch (stoi(word)) {
		case 1: 
			mem.page_size = 100;
			break;
		case 2: 
			mem.page_size = 200;
			break;
		case 3: 
			mem.page_size = 400;
			break;
		default: std::cout << "\tYou put a wrong number.\n";
			 return 0;
	}
	mem.rest_cap = (mem.capacity / mem.page_size) * mem.page_size;
	std::cout << "memory size = " << mem.rest_cap << '\n';
	std::cout << "memory page size = " << mem.page_size << '\n';
	
	//split the memory multiple segments by page size;
	for (int i = 0; i < mem.capacity/mem.page_size; i++) {
		page pg;
		pg.pid = -1;
		pg.page_num = -1;
		pg.range_s = i * mem.page_size;
		pg.range_e = (i + 1) * mem.page_size - 1;
		mem.pages.push_back(pg);
	}
	
	// print the memory map;
	// print the memory map;
	
	for (int j = 0; j < mem.pages.size(); j++) {
		if (mem.pages[j].pid != -1) {
			if (j == 0) {
				std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\t\tProcess" << mem.pages[j].pid << ", Page " << mem.pages[j].page_num << '\n';
			} else {
				std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\tProcess" << mem.pages[j].pid << ", Page " << mem.pages[j].page_num << '\n';
			}
		} else {
			if (j == 0) {
				std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\t\tFree frame" << '\n';
			} else {
				std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\tFree frame" << '\n';
			}
		}
	}

	//make process_structure from inputfile;
	num_p = input_file[0];

	int idx_next = 1;					// index for next process
	for (int i = 0; i < num_p; i++) {
		proc p;
		p.exit_time = -1;				// set the default exit time from the memory as -1
//		for (; idx_next < input_file.size(); idx_next++ ) {
		p.pid = i+1; // add info from input file to procs
//		std::cout << "pid = " << p.pid << '\n';
		idx_next++;
		p.arrival_time = input_file[idx_next];
		if (p.arrival_time > 100000) {
			std::cout << "\tTIMEOUT! t > 100000" << '\n';
			return 0;
		}
//		std::cout << "arrival_time = " << p.arrival_time << '\n';
		idx_next++;
		p.exe_time = input_file[idx_next];
//		std::cout << "exe_time = " << p.exe_time << '\n';
		idx_next++;
		p.mem_pieces = input_file[idx_next];
//		std::cout << "pieces = " << p.mem_pieces << '\n';
		idx_next++;
		p.total_mem = 0;
		int j = p.mem_pieces;
		while (j > 0) {
			p.mem_size.push_back(input_file[idx_next]);
//			std::cout << "p.mem_size[" << p.mem_pieces - j << "] = " << p.mem_size[p.mem_pieces - j] << '\n';
			p.total_mem += p.mem_size[p.mem_pieces - j];
			j--;
			idx_next++;
		}
		procs.push_back(p);
	}
	for (int i = 0; i < num_p; i++) {
		turnaround_time += (double)procs[i].exe_time;
	}
	// test	
//	std::cout << "base ta_time = " << turnaround_time << '\n';
//	for (int i = 0; i < num_p; i++) {
//		std::cout << "process " << procs[i].pid << '\n' << "arrival " << procs[i].arrival_time << ", exe time " << procs[i].exe_time << '\n' << "pieces " << procs[i].mem_pieces << '\n' << "total_mem " << procs[i].total_mem << '\n';
//	};
	// add arrival_events on the timeline
	timeline.push_back(-1);
	int m = 0;
//	std::cout << "timeline[0] = " << timeline[0] << '\n';
	while (timeline[0] == -1) {
		if (procs[m].total_mem <= mem.rest_cap) {
			timeline[0] = procs[m].arrival_time;
			break;
		}
		std::cout << "\tProcess " << m + 1 << " needs " << procs[m].total_mem << " memory space, and it does not fit this " << mem.rest_cap << " memory size." << '\n';
		m++;
	}
	for (int i = m; i < num_p; i++) {
	       	if (procs[i].total_mem > mem.rest_cap) {
			std::cout << "\tProcess " << i + 1 << " needs " << procs[i].total_mem << " memory space, and it does not fit this " << mem.rest_cap << " memory size." << '\n';
			continue;
		}
		if (procs[i - 1].arrival_time != procs[i].arrival_time) {
			timeline.push_back(procs[i].arrival_time);
		}
	}
	timeline.push_back(-1);
	// test timeline
//	for (int i = 0; i < timeline.size(); i++) {
//		std::cout << timeline[i] << " ";
//	}
//	std::cout << '\n';
	// check timeline
	// check at that time, whether arriving events exist	
	
	int i = 0;
	int t_line = timeline.size() + 1;
	while (i < t_line) {
//	for (int i = 0; i < t_line; i++) {
		if (timeline[i] != -1) {
			std::cout << "t = " << timeline[i] << " :" << '\n';
			// put the arrived processes on the input queue
			for (int j = 0; j < procs.size(); j++) {
				if (timeline[i] == procs[j].arrival_time) {
					std::cout << "\tprocess " << j + 1 << " arrived." << '\n';
					p_que.push_back(procs[j].pid);
				}
			}
			// print the input queue
			if (p_que.size() != 0) {
				std::cout << "\tInput Queue : [";
			}
			for (int j = 0; j < p_que.size(); j++) {
				if (j == p_que.size() - 1) {
					std::cout << p_que[j] << "]"<< '\n';
				} else {
					std::cout << p_que[j] << " ";
				}
			}
			// check whether a completed process exists on the memory
			for (int j = 0; j < num_p; j++) {
				if (timeline[i] == procs[j].exit_time) {
					std::cout << "\tprocess " << procs[j].pid << " completes." << '\n';
//					remove process[j] from the memory;	//TODO
					int num_pg = 0;
					for (int k = 0; k < mem.pages.size(); k++) {
						if (mem.pages[k].pid == procs[j].pid) {
							mem.pages[k].pid = -1;
							mem.pages[k].page_num = -1;
							num_pg++;
						}
					}
					mem.rest_cap += (num_pg * mem.page_size);
//					removing process[j] is done! //TODO 
	// print the memory map;
	
					for (int j = 0; j < mem.pages.size(); j++) {
						if (mem.pages[j].pid != -1) {
							if (j == 0) {
								std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\t\tProcess" << mem.pages[j].pid << ", Page " << mem.pages[j].page_num << '\n';
							} else {
								std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\tProcess" << mem.pages[j].pid << ", Page " << mem.pages[j].page_num << '\n';
							}
						} else {
							if (j == 0) {
								std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\t\tFree frame" << '\n';
							} else {
								std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\tFree frame" << '\n';
							}
						}
					}
//--------------------------------
				}
			}
			// move a process from queue to memory

			// add exit time of a moved process on the timeline
			timeline.pop_back();
			for (int k = 0; k < p_que.size(); k++) {
				if (mem.rest_cap >= procs[p_que[k] - 1].total_mem && p_que[k] != 0) {
					std::cout << "\tprocess " << p_que[k] << " to memory." << '\n';
//					place process[j] on the memory;		//TODO
					int proc_mem = procs[p_que[k] - 1].total_mem;	// to check how much memory to be moved after using # of pages
					int num_pg = proc_mem/mem.page_size;		// indicates how many pages is used for the process
					if (proc_mem % mem.page_size != 0) {
						num_pg += 1;
					}
					std::cout << "\tProcess " << p_que[k] << " needs " << num_pg << " pages." << '\n';
					for (int j = 0; j < num_pg; j++) {
						for (int l = 0; l < mem.pages.size(); l++) {
							if (mem.pages[l].pid == -1) {
								mem.pages[l].pid = p_que[k];
								mem.pages[l].page_num = j + 1;
								break;
							}
						}
					}
					mem.rest_cap -= (num_pg * mem.page_size);
	// print the memory map;
					for (int j = 0; j < mem.pages.size(); j++) {
						if (mem.pages[j].pid != -1) {
		
							if (j == 0) {			
								std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\t\tProcess" << mem.pages[j].pid << ", Page " << mem.pages[j].page_num << '\n';
							} else {
								std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\tProcess" << mem.pages[j].pid << ", Page " << mem.pages[j].page_num << '\n';
							}
						} else {
							if (j == 0) {
								std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\t\tFree frame" << '\n';
							} else {
								std::cout << '\t' << mem.pages[j].range_s << "-" << mem.pages[j].range_e << ":\tFree frame" << '\n';
							}
						}
					}
//--------------------------------
//					mem.rest_cap -= procs[p_que[k] - 1].total_mem;
//					std::cout << "\tremained memory : " << mem.rest_cap << '\n';
					procs[p_que[k] - 1].exit_time = procs[p_que[k] - 1].exe_time + timeline[i];
					bool same = true;
					for (int j = 0; j < timeline.size(); j++) {
						if (timeline[j] == procs[p_que[k] - 1].exit_time) {
							same = false;
							break;
						}
					}
					if (same) {
						timeline.push_back(procs[p_que[k] - 1].exit_time);
						if (procs[p_que[k] - 1].exit_time > 100000) {
							std::cout << "\tTIMEOUT! t > 100,000" << '\n';
							return 0;
						}
//						std::cout << "\tnew event is added on time " << procs[p_que[k] - 1].exit_time << '\n';
					}
					turnaround_time += (double)(timeline[i] - procs[p_que[k] - 1].arrival_time);
					std::sort (timeline.begin(), timeline.end());
					p_que.erase (p_que.begin() + k);
					k--;
				}
			}
			
			timeline.push_back(-1);
		} else {
			std::cout << "All done." << '\n';
		}
		t_line = timeline.size();
		i++;
//		std::cout << "\tt_line = " << t_line << ", i = " << i << '\n';
	}
	double ta_time = turnaround_time / num_p;
	std::cout << "turnaround time = " << ta_time << "\t(" << turnaround_time << " / " << num_p << ")" << '\n';
	return 0;
}
