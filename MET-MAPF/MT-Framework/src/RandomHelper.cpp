//
// Created by hiccup on 2020/5/6.
//

#include "../include/RandomHelper.h"

#include <indicators/progress_bar.hpp>
//#include <indicators/progress_spinner.hpp>
#include <indicators/cursor_control.hpp>
#include <easyloggingpp/easylogging++.h>

namespace TestingFramework{

    // bugs fixed!!
    // reference :https://weblogs.asp.net/whaggard/252685;
    // For .so file, you need to declare static member outside the class because otherwise the compiler doesn't know
    // which translation unit (hence object file) the member is supposed to go.
    std::mt19937 RandomHelper::generator;
    std::uniform_int_distribution<uint32_t>RandomHelper::distribution;
    std::uniform_int_distribution<int> RandomHelper::distribution_int;
    std::mt19937 RandomHelper::gene_core;

    void RandomHelper::InitSeed() {
        // This is a special class which produces uniformly-distributed unsigned integers with 32 bits of length.
        // It can produce random numbers either by accessing the operational system's entropy
        // pool via system calls or by using hardware random number generators such as Intel's
        // RdRand when available (but not all implementations of std::random_device allow this).
        // Developers must be warned, however, that even though 32-bit unsigned integers are generated,
        // the entropy of this random number random_generator may be lower than 32 bits.
        std::random_device seed_gen;
        generator = std::mt19937(seed_gen());
        distribution = std::uniform_int_distribution<uint32_t>(0, std::numeric_limits<uint32_t>::max());
    }

    void ProgressBarHelper::setTotalNum(unsigned n) {
        if (n == 0) n = 100;
        total_num = n;
        progress =0;
        step = 100.0 / total_num;
        show_console_cursor(false);
    }

    void ProgressBarHelper::updateProgressBar() {
        std::cout<<"\r\n";
        LOG(INFO)<<"progress:"<<progress<<" epoch info:" << current_epoch << "/" << total_epoch <<std::endl;
        LOG(INFO)<<extra_paint_info<<std::endl;
        bar.set_progress(progress);
        std::cout.flush();

        //std::cout<<std::endl;

        progress+=step;
        if(bar.is_completed())
            show_console_cursor(true);
    }

    void ProgressBarHelper::showProgressBar(){
        std::cout<<"\r\n";
        bar.set_progress(progress);
        std::cout<<std::endl;
        //std::cout.flush();
    }

    ProgressBarHelper ProgressBarHelper::single_instance;
}