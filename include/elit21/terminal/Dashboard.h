#pragma once
#include "elit21/config/Config.h"
#include "elit21/core/Types.h"
#include "elit21/logging/Logger.h"
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
namespace elit21 {class Dashboard{public:Dashboard(TerminalConfig c,RuntimeCounters&n,Logger&l);~Dashboard();void start();void stop();void setStatus(std::string status);void setActivity(std::string activity,double progress=0);private:void run();void render();std::string bar(double value,int width)const;TerminalConfig config_;RuntimeCounters&counters_;Logger&logger_;std::atomic_bool running_{false};std::thread thread_;mutable std::mutex mutex_;std::string status_{"DÉMARRAGE"},activity_{"Initialisation"};double progress_{0};};}
