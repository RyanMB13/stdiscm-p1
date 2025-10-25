#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>
#include <ctime>
using namespace std;

struct Config { int threads = 1, max_n = 100; };

bool read_config(const string& path, Config& c) {
    ifstream in(path);
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        auto p = line.find('#');
        if (p != string::npos) line = line.substr(0, p);
        auto trim = [](string s) {
            auto a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
            return a == string::npos ? "" : s.substr(a, b - a + 1);
        };
        line = trim(line);
        if (line.empty()) continue;
        auto eq = line.find('=');
        if (eq == string::npos) continue;
        string k = trim(line.substr(0, eq)), v = trim(line.substr(eq + 1));
        if (k == "threads") c.threads = stoi(v);
        else if (k == "max_n") c.max_n = stoi(v);
    }
    return true;
}

bool is_prime_parallel(uint32_t n, int w) {
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;

    atomic<bool> found{false};
    vector<thread> workers;
    uint32_t total = 0;

    for (uint32_t d = 3; d * d <= n; d += 2)
        total++;

    uint32_t chunk = (total + w - 1) / w;
    for (int i = 0; i < w; i++) {
        uint32_t start = i * chunk;
        uint32_t end = min<uint32_t>(total, (i + 1) * chunk);
        workers.emplace_back([&, start, end]() {
            for (uint32_t d = 3 + start * 2, k = 0;
                 k < end - start && d * d <= n && !found;
                 k++, d += 2) {
                if (n % d == 0) { found = true; break; }
            }
        });
    }
    for (auto &t : workers) t.join();
    return !found;
}

string now_ts() {
    time_t t = time(0);
    char b[64];
    strftime(b, 64, "%F %T", localtime(&t));
    return b;
}

int main() {
    Config cfg;
    if (!read_config("config.txt", cfg)) {
        cerr << "Error: config.txt not found.\n";
        return 1;
    }

    cfg.threads = min(cfg.threads, 1 << 8);
    cfg.max_n = min(cfg.max_n, 1 << 16);

    cout << "[" << now_ts() << "] RUN START: Variant 4 (Intra + Deferred)\n";

    vector<uint32_t> primes;
    if (cfg.max_n >= 2)
        primes.push_back(2); // handled by main thread

    for (uint32_t n = 3; n <= cfg.max_n; n++)
        if (is_prime_parallel(n, cfg.threads))
            primes.push_back(n);

    sort(primes.begin(), primes.end());
    for (auto n : primes)
        cout << "[" << now_ts() << "] prime=" << n << "\n";

    cout << "[" << now_ts() << "] RUN END\n";
}