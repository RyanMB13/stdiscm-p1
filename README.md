# stdiscm-p1

Build and Run Instructions

You can compile and run each variant manually from the Terminal.

	1.	Open the Terminal and navigate to a variant folder, for example:
        cd "Variant 1 — Range Split + Immediate Print"

    2.	Compile the program:
        g++ -std=c++17 -O2 -pthread main.cpp -o prime_threads

    3.	Run the program:
        ./prime_threads

Configuration File

Each variant uses a config.txt file to define how many threads to use and how many numbers to check.

Example: config.txt
# Prime Finder Configuration
	threads=8
	max_n=50000

Parameters:
	•	threads → number of worker threads to spawn (1–256)
	•	max_n → highest integer to check for primality (2–65536)
