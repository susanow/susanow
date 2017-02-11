

// TODO: Add copylight message
// TODO: Add initial comment for Doxygen

#pragma once


// #include <stdio.h>
// #include <string.h>
// #include <string>
// #include <vector>
//
//
// class ssn_thread {};
// class testwk : public ssn_thread {};
// class testrx : public ssn_thread {};
// class testtx : public ssn_thread {};
//
//
//
// class dta2_thread_raw {
// public:
//     virtual void inc() = 0;
//     virtual void dec() = 0;
// };
//
//
// template <class T>
// class dta2_thread : public dta2_thread_raw {
//     std::vector<ssn_thread*> raw;
// public:
//     void inc() override
//     {
//         raw.push_back(new T);
//     }
//     void dec() override
//     {
//         auto* tmp = raw.back();
//         raw.pop_back();
//         delete tmp;
//     }
// };
//
//
// int main()
// {
//
//     std::vector<dta2_thread_raw*> threads;
//     threads.push_back(new dta2_thread<testwk>);
//     threads.push_back(new dta2_thread<testrx>);
//     threads.push_back(new dta2_thread<testtx>);
//     threads[2]->dec();
//     threads[2]->dec();
//     threads[0]->inc();
//     threads[1]->inc();
//
// }
