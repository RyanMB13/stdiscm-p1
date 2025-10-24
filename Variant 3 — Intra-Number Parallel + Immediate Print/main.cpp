#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <ctime>
using namespace std;

struct Config{int threads=1,max_n=100;};
bool read_config(const string&p,Config&c){
    ifstream in(p);if(!in)return false;string l;
    while(getline(in,l)){auto p=l.find('#');if(p!=string::npos)l=l.substr(0,p);
        auto trim=[](string s){auto a=s.find_first_not_of(" \t\r\n"),b=s.find_last_not_of(" \t\r\n");return a==string::npos?"":s.substr(a,b-a+1);};
        l=trim(l);if(l.empty())continue;auto e=l.find('=');if(e==string::npos)continue;
        string k=trim(l.substr(0,e)),v=trim(l.substr(e+1));if(k=="threads")c.threads=stoi(v);else if(k=="max_n")c.max_n=stoi(v);}return true;}
bool is_prime_parallel(uint32_t n,int w){
    if(n<2)return false;if(n%2==0)return n==2;
    atomic<bool>found{false};vector<thread>ts;uint32_t total=0;for(uint32_t d=3;d*d<=n;d+=2)total++;
    uint32_t chunk=(total+w-1)/w;for(int i=0;i<w;i++){uint32_t start=i*chunk;uint32_t end=min<uint32_t>(total,(i+1)*chunk);
        ts.emplace_back([&,start,end]{uint32_t d=3+start*2;for(uint32_t k=0;k<end-start&&d*d<=n&&!found;k++,d+=2)if(n%d==0){found=true;break;}});}
    for(auto&t:ts)t.join();return !found;}
string now_ts(){time_t t=time(0);char b[64];strftime(b,64,"%F %T",localtime(&t));return b;}
int main(){
    Config c;if(!read_config("config.txt",c)){cerr<<"no config\n";return 1;}
    c.threads=min(c.threads,1<<8);c.max_n=min(c.max_n,1<<16);
    cout<<"["<<now_ts()<<"] RUN START: Variant 3 (Intra + Immediate)\n";
    mutex m;for(uint32_t n=2;n<=c.max_n;n++)if(is_prime_parallel(n,c.threads)){lock_guard<mutex>l(m);cout<<"["<<now_ts()<<"] tid="<<this_thread::get_id()<<" prime="<<n<<"\n";}
    cout<<"["<<now_ts()<<"] RUN END\n";
}