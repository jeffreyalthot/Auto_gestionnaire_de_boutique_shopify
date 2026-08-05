#include "elit21/terminal/Dashboard.h"
#include "elit21/util/StringUtil.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#endif
namespace elit21 {
namespace {
void prepareTerminal() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (output != INVALID_HANDLE_VALUE && GetConsoleMode(output, &mode)) {
        SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
    }
#endif
}
}
Dashboard::Dashboard(TerminalConfig c,RuntimeCounters&n,Logger&l):config_(c),counters_(n),logger_(l){}Dashboard::~Dashboard(){stop();}void Dashboard::start(){if(running_)return;prepareTerminal();running_=true;std::cout<<"\x1b[2J\x1b[H\x1b[?25l";thread_=std::thread([this]{run();});}void Dashboard::stop(){bool was_running=running_.exchange(false);if(thread_.joinable())thread_.join();if(was_running)std::cout<<"\x1b[?25h\n"<<std::flush;}void Dashboard::setStatus(std::string s){std::lock_guard l(mutex_);status_=std::move(s);}void Dashboard::setActivity(std::string a,double p){std::lock_guard l(mutex_);activity_=std::move(a);progress_=std::clamp(p,0.0,1.0);}std::string Dashboard::bar(double v,int w)const{int f=static_cast<int>(v*w);return"["+std::string(static_cast<std::size_t>(f),'#')+std::string(static_cast<std::size_t>(w-f),'.')+"]";}
void Dashboard::run(){while(running_){render();std::this_thread::sleep_for(std::chrono::milliseconds(config_.refresh_ms));}}void Dashboard::render(){std::string st,act;double pr;{std::lock_guard l(mutex_);st=status_;act=activity_;pr=progress_;}std::ostringstream o;o<<"\x1b[H\x1b[0m";o<<"+============================================================================+\n";o<<"| ELIT21 SHOPIFY x ALIEXPRESS AUTONOMOUS MANAGER                            |\n";o<<"+============================================================================+\n";o<<"| Etat: "<<std::left<<std::setw(68)<<st<<"|\n";o<<"| Regle prix: coût fournisseur + 100% AVANT livraison, puis livraison       |\n";o<<"+----------------------------------------------------------------------------+\n";auto line=[&](const char*n,auto v){o<<"| "<<std::left<<std::setw(31)<<n<<": "<<std::right<<std::setw(12)<<v<<std::setw(31)<<""<<"|\n";};line("Produits analyses",counters_.products_scanned);line("Produits acceptes",counters_.products_accepted);line("Produits rejetes",counters_.products_rejected);line("Produits publies",counters_.products_published);line("Stocks synchronises",counters_.inventory_updates);line("Prix synchronises",counters_.price_updates);line("Commandes Shopify",counters_.orders_received);line("Commandes AliExpress",counters_.supplier_orders);line("Expediees",counters_.shipped);line("Livrees",counters_.delivered);line("Evenements acceptes",counters_.accepted_events);line("Evenements rejetes",counters_.rejected_events);line("Erreurs",counters_.errors);o<<"| Profit brut estime CAD          : "<<std::right<<std::setw(12)<<std::fixed<<std::setprecision(2)<<counters_.estimated_profit_cad<<std::setw(31)<<""<<"|\n";o<<"+----------------------------------------------------------------------------+\n";o<<"| Activite: "<<std::left<<std::setw(64)<<util::sanitizeTerminal(act).substr(0,64)<<"|\n";o<<"| Progression: "<<bar(pr,48)<<" "<<std::setw(3)<<static_cast<int>(pr*100)<<"%       |\n";o<<"+----------------------------------------------------------------------------+\n";auto logs=logger_.recent(static_cast<std::size_t>(config_.event_lines));for(int i=0;i<config_.event_lines;++i){std::string x=i<static_cast<int>(logs.size())?logs[static_cast<std::size_t>(i)]:"";if(x.size()>74)x=x.substr(x.size()-74);o<<"| "<<std::left<<std::setw(74)<<x<<" |\n";}o<<"+============================================================================+\n";std::cout<<o.str()<<std::flush;}}
