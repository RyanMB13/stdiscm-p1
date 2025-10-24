#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <thread>
#include <mutex>
using namespace std;

struct Config { int threads=1, max_n=100; };

bool read_config(const string& path, Config& c){
    ifstream in(path); if(!in) return false;
    string line;
    while(getline(in,line)){
        auto p=line.find('#'); if(p!=string::npos) line=line.substr(0,p);
        auto trim=[](string s){auto a=s.find_first_not_of(" \t\r\n"),b=s.find_last_not_of(" \t\r\n");return a==string::npos?"":s.substr(a,b-a+1);};
        line=trim(line); if(line.empty()) continue;
        auto eq=line.find('='); if(eq==string::npos) continue;
        string k=trim(line.substr(0,eq)),v=trim(line.substr(eq+1));
        if(k=="threads") c.threads=stoi(v); else if(k=="max_n") c.max_n=stoi(v);
    } return true;
}

bool is_prime(uint32_t n){
    if(n<2) return false; if(n%2==0) return n==2;
    for(uint32_t d=3; d*d<=n; d+=2) if(n%d==0) return false;
    return true;
}

string now_ts(){ time_t t=time(0); char b[64]; strftime(b,64,"%F %T",localtime(&t)); return b; }

int main(){
    Config cfg; if(!read_config("config.txt",cfg)){cerr<<"no config\n";return 1;}
    cfg.threads=min(cfg.threads,1<<8); cfg.max_n=min(cfg.max_n,1<<16);
    cout<<"["<<now_ts()<<"] RUN START: Variant 1 (Range Split + Immediate)\n";
    mutex print_mx; vector<thread> th;
    uint64_t odds=((cfg.max_n-3)/2)+1,chunk=(odds+cfg.threads-1)/cfg.threads;
    if(cfg.max_n>=2) cout<<"prime=2\n";
    for(int t=0;t<cfg.threads;t++){
        uint64_t s=t*chunk,e=min<uint64_t>(odds,(t+1)*chunk);
        th.emplace_back([&,s,e]{for(uint64_t i=s;i<e;i++){uint32_t n=3+i*2;if(is_prime(n)){lock_guard<mutex>l(print_mx);cout<<"["<<now_ts()<<"] tid="<<this_thread::get_id()<<" prime="<<n<<"\n";}}});
    }
    for(auto&x:th)x.join();
    cout<<"["<<now_ts()<<"] RUN END\n";
}