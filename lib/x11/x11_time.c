//========================================================================
// GLFW - An OpenGL framework
// File:        x11_time.c
// Platform:    X11 (Unix)
// API version: 2.4
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2003 Marcus Geelnard
//
// GLFW is provided "as-is", without any express or implied warranty,
// without even the implied warranty of fitness for a particular purpose.
// In no event will the author be held liable for any damage araising from
// the use of GLFW.
//
// Permission is granted to anyone to use GLFW for any purpose, including
// commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. You must not claim that you wrote GLFW.
// 2. If you modify any parts of GLFW, it must be clearly stated which
//    modifications have been made, and that those modifications are not
//    the work of the author of GLFW.
// 3. If you use GLFW in a product, an acknowledgment in the product
//    documentation would be appreciated, but is not required.
// 4. This notice may not be removed or altered from any source
//    distribution of GLFW.
//
// Marcus Geelnard
// marcus.geelnard at home.se
//------------------------------------------------------------------------
// $Id: x11_time.c,v 1.3 2003-10-20 21:41:04 marcus256 Exp $
//========================================================================

#include "internal.h"


//************************************************************************
// TO BE ADDED:
//
// - SUN UltraSPARC 63-bit CPU tick counter support
//   #if defined(ARCH_SPARCV9) && defined(__GNUC__)
//       unsigned long t_64;
//       asm volatile ("rd %%tick,%0\n" : "=r" (t_64));
//   #endif
//
// - Alpha RPCC support (?)
//   According to Tim Goodwin, the upper 32 bits of the 64-bit "processor
//   cycle counter" (PCC) is used by Digital Unix for process timing, so
//   we can only use the lower 32 bits of the counter. Thus we need to
//   handle wrap-arounds. On other OS:es that do not use the upper 32 bits
//   of the PCC, we could use all the 64 bits, but with the wrap-around
//   handling it should not be necessary. Possible wrap around handling:
//   combine RPCC with gettimeofday().
//   #if defined(__alpha__) && defined(__GNUC__)
//       unsigned long t_64;
//       unsigned int  t_32;
//       asm volatile ("rpcc %0\n" : "=r" (t_64));
//       t_32 = (unsigned int) t_64;
//   #endif
//
//************************************************************************

// We use the __i386 define later in the code. Check if there are any
// other defines that hint that we are compiling for 32-bit x86
#ifndef __i386
#if defined(__i386__) || defined(i386) || defined(X86)
#define __i386
#endif
#endif // __i386

// Should we use inline x86 assembler?
#if defined(__i386) && defined(__GNUC__)
#define _USE_X86_ASM
#endif



//************************************************************************
//****           "Intel Mobile CPU identification database"           ****
//************************************************************************

#ifdef _USE_X86_ASM

// Structure for holding Intel CPU identification comparison data
typedef struct {
    unsigned int BrandID;       // Bits 7-0 of EBX for CPUID 1
    unsigned int Signature_Min; // Bits 11-0 of EAX for CPUID 1
    unsigned int Signature_Max; // --"--
} _GLFWcpuid;

// List of known Intel Mobile CPU identifications
// NOTE: This list is untested and probably incomplete
#define _NUM_KNOWN_MOBILE_CPUS 10
static const _GLFWcpuid _glfw_mobile_cpus[ _NUM_KNOWN_MOBILE_CPUS ] = {
    // Mobile Pentium / Mobile Pentium MMX
    { 0x00, 0x570, 0x58F },

    // Mobile Pentium II / Mobile Celeron (II)
    { 0x00, 0x66A, 0x66A },

    // Mobile Pentium II / Mobile Celeron (II)
    { 0x00, 0x66D, 0x66D },

    // Mobile Pentium III (? - used in IBM ThinkPad)
    { 0x02, 0x681, 0x681 },

    // Mobile Pentium III (? - used in IBM ThinkPad)
    { 0x02, 0x68A, 0x68A },

    // Mobile Pentium III (Banias according to WCPUID?!)
    { 0x02, 0x690, 0x69F },

    // Mobile Pentium III-M
    { 0x06, 0x000, 0xFFF },

    // Mobile Celeron (III?)
    { 0x07, 0x000, 0xFFF },

    // Mobile Pentium 4-M
    { 0x0E, 0xF13, 0xFFF },

    // Mobile Celeron (4?)
    { 0x0F, 0x000, 0xFFF }
};

#endif // _USE_X86_ASM



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

// Functions for accessing upper and lower parts of 64-bit integers
// (Note: These are endian dependent, but ONLY used on x86 platforms!)
#define _HIGH(x) ((unsigned int*)&x)[1]
#define _LOW(x)  *((unsigned int*)&x)


//========================================================================
// _glfwCPUID() - Execute x86 CPUID instruction
//========================================================================

static int _glfwCPUID( unsigned int ID, unsigned int *a, unsigned int *b,
    unsigned int *c, unsigned int *d )
{
    // Inline assembly (requires GCC)
#if defined(__i386) && defined(__GNUC__)

    int has_cpuid;
    unsigned int local_a, local_b, local_c, local_d;

    // Detect CPUID support
    asm(
        "pushf\n\t"
        "pop    %%eax\n\t"
        "movl   %%eax,%%ebx\n\t"
        "xorl   $0x00200000,%%eax\n\t"
        "push   %%eax\n\t"
        "popf\n\t"
        "pushf\n\t"
        "pop    %%eax\n\t"
        "xorl   %%eax,%%ebx\n\t"
        "movl   %%eax,%0\n\t"
        : "=m" (has_cpuid)
        :
        : "%eax", "%ebx"
    );
    if( !has_cpuid )
    {
        return GL_FALSE;
    }

    // Execute CPUID
    asm(
        "movl   %4,%%eax\n\t"
        "cpuid\n\t"
        "movl   %%eax,%0\n\t"
        "movl   %%ebx,%1\n\t"
        "movl   %%ecx,%2\n\t"
        "movl   %%edx,%3\n\t"
        : "=m" (local_a), "=m" (local_b), "=m" (local_c), "=m" (local_d)
        : "m" (ID)
        : "%eax", "%ebx", "%ecx", "%edx"
    );

    *a = local_a;
    *b = local_b;
    *c = local_c;
    *d = local_d;
    return GL_TRUE;

#else

    return GL_FALSE;

#endif
}


//========================================================================
// _glfwHasRDTSC() - Check for RDTSC availability AND usefulness
//========================================================================

static int _glfwHasRDTSC( void )
{
#ifdef _USE_X86_ASM

    unsigned int cpu_name1, cpu_name2, cpu_name3;
    unsigned int cpu_signature, cpu_brandID;
    unsigned int max_base, feature_flags, has_rdtsc, has_htt, num_logical;
    unsigned int dummy;

    // Get processor vendor string (will return 0 if CPUID is not
    // supported)
    if( !_glfwCPUID( 0, &max_base, &cpu_name1, &cpu_name3, &cpu_name2 ) )
    {
        return GL_FALSE;
    }

    // Does the processor support base CPUID function 1?
    if( max_base < 1 )
    {
        return GL_FALSE;
    }

    // Get CPU capabilities, CPU Brand ID & CPU Signature
    _glfwCPUID( 1, &cpu_signature, &cpu_brandID, &dummy, &feature_flags );
    cpu_signature &= 0x00000fff;
    num_logical    = (cpu_brandID & 0x00ff0000) >> 16;
    cpu_brandID   &= 0x000000ff;
    has_rdtsc      = feature_flags & 0x00000010;
    has_htt        = feature_flags & 0x10000000;
    if( num_logical == 0 ) num_logical = 1;

    // Is RDTSC supported?
    if( !has_rdtsc )
    {
        return GL_FALSE;
    }

    // Detect Intel "Mobile" CPU (a bit tricky, this one...)
    // Also detect Intel HTT flag (Hyper-Threading)
    if( cpu_name1 == 0x756e6547 &&  // Genu
        cpu_name2 == 0x49656e69 &&  // ineI
        cpu_name3 == 0x6c65746e )   // ntel
    {
        unsigned int i, max_ext;

        // Method 1: Check if this is a known Intel "Mobile" CPU - disable
        // RDTSC on those since they MAY have a variable core frequency
        // (there is no way of knowing for sure)
        for( i = 0; i < _NUM_KNOWN_MOBILE_CPUS; i ++ )
        {
            if( cpu_brandID   == _glfw_mobile_cpus[ i ].BrandID &&
                cpu_signature >= _glfw_mobile_cpus[ i ].Signature_Min &&
                cpu_signature <= _glfw_mobile_cpus[ i ].Signature_Max )
            {
                return GL_FALSE;
            }
        }

        // Method 2: Search Brand String for "mobile" (should work on
        // Intel P4 processors and later, so this way we should be future
        // compatible)
        _glfwCPUID( 0x80000000, &max_ext, &dummy, &dummy, &dummy );
        if( max_ext >= 0x80000004 )
        {
            char str[ 48+1 ];

            // Get Brand String
            _glfwCPUID( 0x80000002, (unsigned int*)&str[0],
                                    (unsigned int*)&str[4],
                                    (unsigned int*)&str[8],
                                    (unsigned int*)&str[12] );
            _glfwCPUID( 0x80000003, (unsigned int*)&str[16],
                                    (unsigned int*)&str[20],
                                    (unsigned int*)&str[24],
                                    (unsigned int*)&str[28] );
            _glfwCPUID( 0x80000004, (unsigned int*)&str[32],
                                    (unsigned int*)&str[36],
                                    (unsigned int*)&str[40],
                                    (unsigned int*)&str[44] );
            str[ 48 ] = 0;

            // Make lower case
            for( i = 0; i < 48; i ++ )
            {
                if( str[ i ] >= 'A' && str[ i ] <= 'Z' )
                {
                    str[ i ] += 'a' - 'A';
                }
            }

            // Check if "mobile" is in the string
            if( strstr( str, "mobile" ) )
            {
                return GL_FALSE;
            }
        }
    }
    else
    {
        // We don't trust Hyper-Threading info on non-Intel CPUs
        has_htt = 0;
    }

    // Detect AMD PowerNOW! support
    if( cpu_name1 == 0x68747541 &&  // Auth
        cpu_name2 == 0x69746e65 &&  // enti
        cpu_name3 == 0x444d4163 )   // cAMD
    {
        unsigned int max_ext, has_fidctl;

        // Get bit #1 of CPUID 0x80000007 (FID control == AMD PowerNOW!)
        _glfwCPUID( 0x80000000, &max_ext, &dummy, &dummy, &dummy );
        if( max_ext >= 0x80000007 )
        {
            _glfwCPUID( 0x80000007, &dummy, &dummy, &dummy, &has_fidctl );
            if( has_fidctl & 0x00000002 )
            {
                // The CPU is a mobile AMD with support for a variable
                // core frequency - disable RDTSC
                return GL_FALSE;
            }
        }
    }

    // Notes:
    //  - Cyrix/VIA: Does not support a dynamic core frequency (AFAIK)
    //  - Transmeta: LongRun does not affect the TSC frequency, so we do
    //     not have to check for LongRun support :)

    // We do not support RDTSC on SMP systems (but single CPU SMT is OK)
    _glfw_numprocessors( dummy );
    if( has_htt )
    {
        dummy /= num_logical;
    }
    if( dummy > 1 )
    {
        return GL_FALSE;
    }

    // Finally: RDTSC is supported, we are running on a single processor
    // system, and we (hopefully) have a constant TSC frequency
    return GL_TRUE;

#else

    // Not an x86 architecture, or not a supported compiler
    return GL_FALSE;

#endif
}


//------------------------------------------------------------------------
// _RDTSC() - Get CPU cycle count using the x86 RDTSC instruction
//------------------------------------------------------------------------

#if defined(__i386) && defined(__GNUC__)

// Read 64-bit processor Time Stamp Counter
#define _RDTSC( hi, lo ) \
    asm( \
        "rdtsc\n\t" \
        "movl %%edx,%0\n\t" \
        "movl %%eax,%1" \
        : "=m" (hi), "=m" (lo) \
        :  \
        : "%edx", "%eax" \
    );

#else

#define _RDTSC( hi, lo ) {hi=lo=0;}

#endif


//========================================================================
// _glfwInitTimer() - Initialise timer
//========================================================================

void _glfwInitTimer( void )
{

#ifdef _GLFW_HAS_CLOCK_SGI_CYCLE

    struct timespec ts;
    long long counter_res;
    int       counter_size;

    // Calculate counter wrap-around limit
    counter_size = (int) syssgi( SGI_CYCLECNTR_SIZE );
    if( counter_size < 48 )
    {
        // The counter is quite short, wrap-arounds can occur. Calculate
        // the wrap-around limit (in nanoseconds).
        clock_getres( CLOCK_SGI_CYCLE, &ts );
        counter_res = (long long) ts.tv_sec * (long long) 1000000000 +
                      (long long) ts.tv_nsec;
        _glfwTimer.WrapLimit = counter_res *
                               (((long long)1) << counter_size);
    }
    else
    {
        // The counter is very large, do not care about wrap-arounds...
        _glfwTimer.WrapLimit = 0;
    }

    // "Resolution" is 1 ns
    _glfwTimer.Resolution = 1e-9;

    // Set start time for timer
    clock_gettime( CLOCK_SGI_CYCLE, &ts );
    _glfwTimer.t0 = (long long) ts.tv_sec * (long long) 1000000000 +
                    (long long) ts.tv_nsec;

    // Reset wrap-around tracking
    _glfwTimer.t_old  = _glfwTimer.t0;
    _glfwTimer.t_comp = 0;

#elif defined(sun)

    // "Resolution" is 1 ns
    _glfwTimer.Resolution = 1e-9;

    // Set start-time for timer
    _glfwTimer.t0 = (long long) gethrtime();

#else

    struct timeval  tv;

    // Check if we have x86 RDTSC support (CPU clock cycle counter)
    if( _glfwHasRDTSC() )
    {
        long long t1, t2;
        long long c1, c2;

        // RDTSC is available => use it!
        _glfwTimer.HasRDTSC = GL_TRUE;

        // Now we will get the CPU frequency by "clocking" the CPU...

        // We do a little thing here to improve our chanses with the
        // process scheduler: sleep first => after returning from sleep we
        // should have a whole "time-slot" for executing, and are less
        // likely to be interrupted by the scheduler (the critical part is
        // that gettimeofday() and _RDTSC() must execute in serial without
        // interruption).
        _glfwPlatformSleep( 0.02 );

        // "Start clock" - get current time, and current CPU cycle count
        gettimeofday( &tv, NULL );
        _RDTSC( _HIGH(c1), _LOW(c1) );
        t1 = (long long) tv.tv_sec * (long long) 1000000 +
             (long long) tv.tv_usec;

        // Sleep again to get something to measure...
        // (100 ms should be enough)
        _glfwPlatformSleep( 0.100 );

        // "Stop clock" - get current time, and current CPU cycle count
        gettimeofday( &tv, NULL );
        _RDTSC( _HIGH(c2), _LOW(c2) );
        t2 = (long long) tv.tv_sec * (long long) 1000000 +
             (long long) tv.tv_usec;

        // Counter resolution is CPU clock cycle period
        _glfwTimer.Resolution = 1e-6 * (double)(t2-t1) / (double)(c2-c1);

        // Set start-time for timer
        _RDTSC( _HIGH(_glfwTimer.t0), _LOW(_glfwTimer.t0) );
    }
    else
    {
        // RDTSC is not available => use standard timing
        _glfwTimer.HasRDTSC = GL_FALSE;

        // "Resolution" is 1 us
        _glfwTimer.Resolution = 1e-6;

        // Set start-time for timer
        gettimeofday( &tv, NULL );
        _glfwTimer.t0 = (long long) tv.tv_sec * (long long) 1000000 +
                        (long long) tv.tv_usec;
    }

#endif
}


//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetTime() - Return timer value in seconds
//========================================================================

double _glfwPlatformGetTime( void )
{
    long long t;
#ifdef _GLFW_HAS_CLOCK_SGI_CYCLE
    struct timespec ts;
#endif // _GLFW_HAS_CLOCK_SGI_CYCLE

#if defined(_GLFW_HAS_CLOCK_SGI_CYCLE)

    // Get SGI hardware counter value
    clock_gettime( CLOCK_SGI_CYCLE, &ts );
    t = (long long) ts.tv_sec * (long long) 1000000000 +
        (long long) ts.tv_nsec;

    // Handle possible wrap-arounds
    if( _glfwTimer.WrapLimit != 0 )
    {
        // Did we have a wrap-around?
        if( t < _glfwTimer.t_old )
        {
            // Increment wrap-around tracking
            _glfwTimer.t_comp += _glfwTimer.WrapLimit;
        }

        // Save this time
        _glfwTimer.t_old = t;

        // Compensate for all recorded wraparounds
        t += _glfwTimer.t_comp;
    }

#elif defined(sun)

    // Use SUN high resolution timer
    t = (long long) gethrtime();

#else

    // Are we using RDTSC or gettimeofday?
    if( _glfwTimer.HasRDTSC )
    {
        _RDTSC( _HIGH(t), _LOW(t) );
    }
    else
    {
        struct timeval  tv;

        gettimeofday( &tv, NULL );
        t = (long long) tv.tv_sec * (long long) 1000000 +
            (long long) tv.tv_usec;
    }

#endif

    return (double)(t - _glfwTimer.t0) * _glfwTimer.Resolution;
}


//========================================================================
// _glfwPlatformSetTime() - Set timer value in seconds
//========================================================================

void _glfwPlatformSetTime( double t )
{
    long long t0;
#ifdef _GLFW_HAS_CLOCK_SGI_CYCLE
    struct timespec ts;
#endif // _GLFW_HAS_CLOCK_SGI_CYCLE


#if defined(_GLFW_HAS_CLOCK_SGI_CYCLE)

    // Get SGI hardware counter value
    clock_gettime( CLOCK_SGI_CYCLE, &ts );
    t0 = (long long) ts.tv_sec * (long long) 1000000000 +
         (long long) ts.tv_nsec;

    // Reset wrap-around tracking
    _glfwTimer.t_old  = t0;
    _glfwTimer.t_comp = 0;

#elif defined(sun)

    // Use SUN high resolution timer
    t0 = (long long) gethrtime();

#else

    // Are we using RDTSC or gettimeofday?
    if( _glfwTimer.HasRDTSC )
    {
        _RDTSC( _HIGH(t0), _LOW(t0) );
    }
    else
    {
        struct timeval  tv;

        gettimeofday( &tv, NULL );
        t0 = (long long) tv.tv_sec * (long long) 1000000 +
             (long long) tv.tv_usec;
    }

#endif

    // Calulate new starting time
    _glfwTimer.t0 = t0 - (long long)(t/_glfwTimer.Resolution);
}


//========================================================================
// _glfwPlatformSleep() - Put a thread to sleep for a specified amount of
// time
//========================================================================

void _glfwPlatformSleep( double time )
{
    struct timeval  currenttime;
    struct timespec wait;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    long dt_sec, dt_usec;

    // Not all pthread implementations have a pthread_sleep() function. We
    // do it the portable way, using a timed wait for a condition that we
    // will never signal. NOTE: The unistd functions sleep/usleep suspends
    // the entire PROCESS, not a signle thread, which is why we can not
    // use them to implement glfwSleep.

    // Set timeout time, relatvie to current time
    gettimeofday( &currenttime, NULL );
    dt_sec  = (long) time;
    dt_usec = (long) ((time - (double)dt_sec) * 1000000.0);
    wait.tv_nsec = (currenttime.tv_usec + dt_usec) * 1000L;
    if( wait.tv_nsec > 1000000000L )
    {
        wait.tv_nsec -= 1000000000L;
        dt_sec ++;
    }
    wait.tv_sec  = currenttime.tv_sec + dt_sec;

    // Initialize condition and mutex objects
    pthread_mutex_init( &mutex, NULL );
    pthread_cond_init( &cond, NULL );

    // Do a timed wait
    pthread_mutex_lock( &mutex );
    pthread_cond_timedwait( &cond, &mutex, &wait );
    pthread_mutex_unlock( &mutex );

    // Destroy condition and mutex objects
    pthread_mutex_destroy( &mutex );
    pthread_cond_destroy( &cond );
}
