#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// intermediate form of input file
std::vector<int> input_file;

// number of processes
int num_p;

// process struct
struct proc {			
	int pid;
	int arrival_time;
	int exe_time;
	int mem_pieces;
	std::vector<int> mem_size;
};

// all processes
std::vector<proc> procs;	

// event timeline, process arrival, process completion
std::vector<int> timeline;

// page struct
struct page {
	int pid;
	int page_num;
};

// whole memory struct
struct memory {
	int size;
	int selected_page;
	int page_size;
	std::vector<page> pages;
} mem;

// process queue
struct p_queue {
	int num_p;
	std::vector<int> p_que;
};

int turnaround_time;

/*
proc create_proc (input_file in) {
	proc p;
	for (int i = 1; i < in.size(); i++) {
	}
	return p;
}
*/

// load a process on the memory
//place_process (process_structure, memory) {
//}

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
	mem.size = stoi(word);
	std::cout << "mem.size = " << mem.size << '\n';
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
	std::cout << "mem.page_size = " << mem.page_size << '\n';
	
	//split the memory multiple segments by page size;

	//make process_structure from inputfile;
	num_p = input_file[0];

	int idx_next = 1;					// index for next process
	for (int i = 0; i < num_p; i++) {
		proc p;
//		for (; idx_next < input_file.size(); idx_next++ ) {
		p.pid = i+1; // add info from input file to procs
//		std::cout << "pid = " << p.pid << '\n';
		idx_next++;
		p.arrival_time = input_file[idx_next];
//		std::cout << "arrival_time = " << p.arrival_time << '\n';
		idx_next++;
		p.exe_time = input_file[idx_next];
//		std::cout << "exe_time = " << p.exe_time << '\n';
		idx_next++;
		p.mem_pieces = input_file[idx_next];
//		std::cout << "pieces = " << p.mem_pieces << '\n';
		idx_next++;
		for (int j = 0; j < p.mem_pieces; j++) {
			p.mem_size.push_back(input_file[idx_next + j]);
			idx_next++;
//			std::cout << "p.mem_size[" << j << "] = " << p.mem_size[j] << '\n';
		}
		procs.push_back(p);
	}
	
	for (int i = 0; i < num_p; i++) {
		std::cout << "process " << procs[i].pid << '\n' << "arrival " << procs[i].arrival_time << ", exe time " << procs[i].exe_time << '\n' << "pieces " << procs[i].mem_pieces << '\n';
	};

	for (int i = 0; i < num_p; i++) {
		timeline.push_back(procs[i].arrival_time);
	}

	for (int i = 0; i < timeline.size(); i++) {
		std::cout << timeline[i] << " ";
	}
/*	-------------------
 *
	adjust virtual_timeline;
		read arrival time;
		add to the virtual_timeline;	// all of them will be an event
	follow_timeline;
	meet an event;
		if (arrival_time == min(arrival_time, complete_process)) {
			add process to the queue;
		} else {
			empty the pages occupied by completed_process;
			turnaround_time += execution time;
		}
		check the 1st process of queue can be placed on memory;
			if (yes)
				place it;							//TODO another function
				add an event on timeline = current time + execution time;
				turnaround_time += (current time - arrivaltime);		// wait_time
				empty it from the queue;
			else (no)
				check the next process;						//TODO could be changed
				until the end of queue;
	if (no more timeline)
		print turnaround_time;
*/	
	return 0;
}
