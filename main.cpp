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


    {
        ScopeTimer delay( "Seconds to complete delay test:" );

        auto thread_pool = std::make_shared< tpl::pool >( 100 );

        auto func = []( double sec ){ std::this_thread::sleep_for( std::chrono::duration<double>(sec) ); };

        size_t i = 0;
        while( i < 1000 )
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

    {
        ScopeTimer delay( "Seconds to complete loop test:" );

        std::vector< int > vec( 10000, 1 );

        struct Sum
        {
            int sum;

            Sum(): sum{0} { }
            void operator()(int n) { sum += n; }
        };

        tpl::pool full_pool;
        tpl::pool single_pool( 1 );

        tpl::for_each( full_pool, vec.begin(), vec.end(), []( int& i ){ i+= 3; } );
        auto sum = tpl::for_each( single_pool, vec.begin(), vec.end(), Sum() );

        std::cout << "sum = " << sum.sum << std::endl;
    }


    return 0;
}
