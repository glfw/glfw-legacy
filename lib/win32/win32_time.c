//========================================================================
// GLFW - An OpenGL framework
// File:        win32_time.c
// Platform:    Windows
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
// $Id: win32_time.c,v 1.2 2003-02-02 21:06:21 marcus256 Exp $
//========================================================================

#include "internal.h"


//************************************************************************
// NOTE: Inline assembly is not supported natively by the free Borland C++
// Builder compiler, since it requires the commercial Borland TASM
// assembly program for such functionality. Hence, RDTSC is not supported
// by GLFW when compiling with Borland.
//************************************************************************



// We use the __i386 define later in the code. Check if there are any
// other defines that hint that we are compiling for 32-bit x86.
#ifndef __i386
#if defined(__i386__) || defined(i386) || defined(X86) || defined(_M_IX86)
#define __i386
#endif
#endif // __i386

// Should we use inline x86 assembler?
#if defined(__i386) && (defined(__GNUC__) || defined(__LCC__) || \
                        defined(_MSC_VER))
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

#ifdef _USE_X86_ASM

// *LCC BUG* Turn off optimization, or register vs stack allocation will
// be screwed up in the following assembler sections
#ifdef __LCC__
 #if __LCCOPTIMLEVEL == 1
  #define _OLD_LCCOPTIMLEVEL 1
 #else
  #define _OLD_LCCOPTIMLEVEL 0
 #endif
 #pragma optimize(0)
#endif

static int _glfwCPUID( unsigned int ID, unsigned int *a, unsigned int *b,
    unsigned int *c, unsigned int *d )
{
    int has_cpuid;
    unsigned int local_a, local_b, local_c, local_d;

    // Inline assembly - GCC version
#if defined(__i386) && defined(__GNUC__)

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


    // Inline assembly - LCC version
#elif defined(__i386) && defined(__LCC__)

    unsigned int local_ID = ID;

    // Detect CPUID support
    _asm( "\tpushf" );
    _asm( "\tpop    %eax" );
    _asm( "\tmovl   %eax,%ebx" );
    _asm( "\txorl   $0x00200000,%eax" );
    _asm( "\tpush   %eax" );
    _asm( "\tpopf" );
    _asm( "\tpushf" );
    _asm( "\tpop    %eax" );
    _asm( "\txorl   %ebx,%eax" );
    _asm( "\tmovl   %eax,%has_cpuid" );
    if( !has_cpuid )
    {
        return GL_FALSE;
    }

    // *LCC BUG* Fist assembler instruction directly after an if-statement
    // ends up _within_ the if-clause (in this case => is never executed!)
    // To solve this, a "nop" instruction is inserted here, otherwise the
    // "movl" instruction would never be executed.
    _asm( "\tnop" );

    // Execute CPUID
    _asm( "\tmovl   %local_ID,%eax" );
    _asm( "\tcpuid" );
    _asm( "\tmovl   %eax,%local_a" );
    _asm( "\tmovl   %ebx,%local_b" );
    _asm( "\tmovl   %ecx,%local_c" );
    _asm( "\tmovl   %edx,%local_d" );


    // Inline assembly - MS Visual C++ version
#elif defined(__i386) && defined(_MSC_VER)

    // Detect CPUID support
    __asm
    {
        pushfd
        pop    eax
        mov    ebx,eax
        xor    eax,0x00200000
        push   eax
        popfd
        pushfd
        pop    eax
        xor    eax, ebx
        mov    [has_cpuid],eax
    }
    if( !has_cpuid )
    {
        return GL_FALSE;
    }

    // Execute CPUID
    __asm
    {
        mov   eax, [ID]
        cpuid
        mov   [local_a], eax
        mov   [local_b], ebx
        mov   [local_c], ecx
        mov   [local_d], edx
    }

#endif

    // Common code for all compilers
#ifdef _USE_X86_ASM

    *a = local_a;
    *b = local_b;
    *c = local_c;
    *d = local_d;
    return GL_TRUE;

#else

    return GL_FALSE;

#endif // _USE_X86_ASM
}

// *LCC BUG* Restore old optimization level
#ifdef __LCC__
 #if _OLD_LCCOPTIMLEVEL == 1
  #pragma optimize(1)
 #endif
#endif

#endif // _USE_X86_ASM


//========================================================================
// _glfwHasRDTSC() - Check for RDTSC availability AND usefulness
//========================================================================

static int _glfwHasRDTSC( void )
{
#ifdef _USE_X86_ASM

    unsigned int cpu_name1, cpu_name2, cpu_name3;
    unsigned int cpu_signature, cpu_brandID;
    unsigned int has_rdtsc, max_base;
    unsigned int dummy;
    SYSTEM_INFO  si;

    // We do not support RDTSC on SMP systems (SMT is OK though)
    GetSystemInfo( &si );
    if( si.dwNumberOfProcessors > 1 )
    {
        return GL_FALSE;
    }

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
    _glfwCPUID( 1, &cpu_signature, &cpu_brandID, &dummy, &has_rdtsc );
    cpu_signature &= 0x00000fff;
    cpu_brandID   &= 0x000000ff;
    has_rdtsc     &= 0x00000010;

    // Is RDTSC supported?
    if( !has_rdtsc )
    {
        return GL_FALSE;
    }

    // Detect Intel "Mobile" CPU (a bit tricky, this one...)
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

    // Finally: RDTSC is supported, we are running on a single processor
    // system, and we (hopefully) have a constant TSC frequency
    return GL_TRUE;

#else

    // Not an x86 architecture, or not a supported compiler
    return GL_FALSE;

#endif
}


//------------------------------------------------------------------------
// _RDTSC() - Get CPU cycle count using the RDTSC instruction
//------------------------------------------------------------------------

#if defined(__i386) && defined(__GNUC__)

// Read 64-bit processor Time Stamp Counter - GCC version
#define _RDTSC( hi, lo ) \
    asm( \
        "rdtsc\n\t" \
        "movl %%edx,%0\n\t" \
        "movl %%eax,%1" \
        : "=m" (hi), "=m" (lo) \
        :  \
        : "%edx", "%eax" \
    );

#elif defined(__i386) && defined(__LCC__)

// Read 64-bit processor Time Stamp Counter - LCC version
#define _RDTSC( hi, lo ) \
{ \
    DWORD _hi, _lo; \
    _asm( "\trdtsc" ); \
    _asm( "\tmovl %edx,%_hi" ); \
    _asm( "\tmovl %eax,%_lo" ); \
    hi = _hi; \
    lo = _lo; \
}

#elif defined(__i386) && defined(_MSC_VER)

// Read 64-bit processor Time Stamp Counter - MSVC version
#define _RDTSC( hi, lo ) \
{ \
    DWORD _hi, _lo; \
    __asm { rdtsc } \
    __asm { mov [_hi],edx } \
    __asm { mov [_lo],eax } \
    hi = _hi; \
    lo = _lo; \
}

#else

#define _RDTSC( hi, lo ) {hi=lo=0;}

#endif



//========================================================================
// _glfwInitTimer() - Initialise timer
//========================================================================

void _glfwInitTimer( void )
{
    __int64 freq, t1_64, t2_64, c1, c2;
    int     t0, t1, t2;
    double  dt;
    DWORD   OldPri;
    HANDLE  Process;

    // Check if we have a performance counter
    if( QueryPerformanceFrequency( (LARGE_INTEGER *)&freq ) )
    {
        // Performance counter is available => use it!
        _glfwTimer.HasPerformanceCounter = GL_TRUE;

        // Counter resolution is 1 / counter frequency
        _glfwTimer.Resolution = 1.0 / (double)freq;

        // Set start time for timer
        QueryPerformanceCounter( (LARGE_INTEGER *)&_glfwTimer.t0_64 );
    }
    else
    {
        // No performace counter available => use the tick counter
        _glfwTimer.HasPerformanceCounter = GL_FALSE;

        // Counter resolution is 1 ms
        _glfwTimer.Resolution = 0.001;

        // Set start time for timer
        _glfwTimer.t0_32 = GetTickCount();
    }

    // Check if we have x86 RDTSC (CPU clock cycle counter)
    if( _glfwHasRDTSC() )
    {
        // RDTSC is available => use it!
        _glfwTimer.HasRDTSC = GL_TRUE;

        // Set process Priority Class to realtime
        Process = GetCurrentProcess();
        OldPri = GetPriorityClass( Process );
        SetPriorityClass( Process, REALTIME_PRIORITY_CLASS );

        // Get CPU frequency
        if( _glfwTimer.HasPerformanceCounter )
        {
            // User performance counter to "clock" the CPU
            QueryPerformanceCounter( (LARGE_INTEGER *) &t1_64 );
            _RDTSC( _HIGH(c1), _LOW(c1) );
            do
            {
                QueryPerformanceCounter( (LARGE_INTEGER *) &t2_64 );
                _RDTSC( _HIGH(c2), _LOW(c2) );
                dt = (double) (t2_64-t1_64) * _glfwTimer.Resolution;
            }
            while( dt < 0.03 );
        }
        else
        {
            // User low performance GetTickCount to "clock" the CPU. This
            // requires a longer clocking time to be reasonably accurate.
            t0 = GetTickCount();
            do
            {
                t1 = GetTickCount();
            } while( t0 == t1 );
            _RDTSC( _HIGH(c1), _LOW(c1) );
            do
            {
                t2 = GetTickCount();
                _RDTSC( _HIGH(c2), _LOW(c2) );
            }
            while( (t2-t1) < 1000 );
            dt = (double) (t2-t1) * 0.001;
        }

        // Restore old Priority Class
        SetPriorityClass( Process, OldPri );

        // Counter resolution is CPU clock cycle period
        _glfwTimer.Resolution = dt / (double)(c2-c1);

        // Set start-time for timer
        _RDTSC( _HIGH(_glfwTimer.t0_64), _LOW(_glfwTimer.t0_64) );
    }
    else
    {
        // RDTSC is not available
        _glfwTimer.HasRDTSC = GL_FALSE;
    }
}


//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetTime() - Return timer value in seconds
//========================================================================

double _glfwPlatformGetTime( void )
{
    double  t;
    __int64 t_64;

    // Are we using RDTSC, the performance counter or GetTickCount?
    if( _glfwTimer.HasRDTSC )
    {
        _RDTSC( _HIGH(t_64), _LOW(t_64) );
        t = (double)(t_64 - _glfwTimer.t0_64);
    }
    else if( _glfwTimer.HasPerformanceCounter )
    {
        QueryPerformanceCounter( (LARGE_INTEGER *)&t_64 );
        t =  (double)(t_64 - _glfwTimer.t0_64);
    }
    else
    {
        t = (double)(GetTickCount() - _glfwTimer.t0_32);
    }

    // Calculate the current time in seconds
    return t * _glfwTimer.Resolution;
}


//========================================================================
// _glfwPlatformSetTime() - Set timer value in seconds
//========================================================================

void _glfwPlatformSetTime( double t )
{
    __int64 t_64;

    // Are we using RDTSC, the performance counter or GetTickCount?
    if( _glfwTimer.HasRDTSC )
    {
        _RDTSC( _HIGH(t_64), _LOW(t_64) );
        _glfwTimer.t0_64 = t_64 - (__int64)(t/_glfwTimer.Resolution);
    }
    else if( _glfwTimer.HasPerformanceCounter )
    {
        QueryPerformanceCounter( (LARGE_INTEGER *)&t_64 );
        _glfwTimer.t0_64 = t_64 - (__int64)(t/_glfwTimer.Resolution);
    }
    else
    {
        _glfwTimer.t0_32 = GetTickCount() - (int)(t*1000.0);
    }
}


//========================================================================
// _glfwPlatformSleep() - Put a thread to sleep for a specified amount of
// time
//========================================================================

void _glfwPlatformSleep( double time )
{
    DWORD t;

    t = (DWORD)(time*1000.0 + 0.5);
    Sleep( t > 0 ? t : 1 );
}
