#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>

#include "pool/thread_pool.hpp"

struct ScopeTimer
{
    std::chrono::time_point< std::chrono::high_resolution_clock > start_time;
    std::string message;

    ScopeTimer( const std::string& m ) : start_time( std::chrono::high_resolution_clock::now() ), message( m )
    {
        /*  */
    }

    ~ScopeTimer()
    {
        std::chrono::duration<double> timediff = std::chrono::high_resolution_clock::now() - start_time;
        std::cout << message << " " << timediff.count() << std::endl;
    }
};

int main()
{
    ScopeTimer scope_timer( "Seconds to complete program:" );

    //std::vector< tpl::future<void> > futures_vec;

    auto thread_pool = std::make_shared< tpl::pool >( 4 );
    {
        auto func = []( double sec ){ std::this_thread::sleep_for( std::chrono::duration<double>(sec) ); };

        size_t i = 0;
        while( i < 10000 )
        {
            thread_pool->submit( func, i < thread_pool->num_threads() ? double(i)/double(thread_pool->num_threads()) : 1.0 ).release();
            //thread_pool->submit( func, 1.0 ).release();
            ++i;
        }

        while( !thread_pool->empty() )
        {
            std::cout << "There are " << std::setw(7) << thread_pool->size() << " jobs remaining in queue\r" << std::flush;
            std::this_thread::sleep_for( std::chrono::duration<double>(0.01) );
        }

        std::cout << std::endl;
    }

    return 0;
}
