////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef SFML_THREAD_HPP
#define SFML_THREAD_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Config.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <cstdlib>


namespace sf
{
namespace priv
{
    class ThreadImpl;
    struct ThreadFunc;
}

////////////////////////////////////////////////////////////
/// \brief Utility class to manipulate threads
///
////////////////////////////////////////////////////////////
class SFML_API Thread : NonCopyable
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Construct the thread from a functor with no argument
    ///
    /// This constructor works for function objects, as well
    /// as free function.
    ///
    /// Use this constructor for this kind of function:
    /// \code
    /// void function();
    ///
    /// // --- or ----
    ///
    /// struct Functor
    /// {
    ///     void operator()();
    /// };
    /// \endcode
    /// Note: this does *not* run the thread, use Run().
    ///
    /// \param function Functor or free function to use as the entry point of the thread
    ///
    ////////////////////////////////////////////////////////////
    template <typename F>
    Thread(F function);

    ////////////////////////////////////////////////////////////
    /// \brief Construct the thread from a functor with an argument
    ///
    /// This constructor works for function objects, as well
    /// as free function.
    /// It is a template, which means that the argument can
    /// have any type (int, std::string, void*, Toto, ...).
    ///
    /// Use this constructor for this kind of function:
    /// \code
    /// void function(int arg);
    ///
    /// // --- or ----
    ///
    /// struct Functor
    /// {
    ///     void operator()(std::string arg);
    /// };
    /// \endcode
    /// Note: this does *not* run the thread, use Run().
    ///
    /// \param function Functor or free function to use as the entry point of the thread
    /// \param argument argument to forward to the function
    ///
    ////////////////////////////////////////////////////////////
    template <typename F, typename A>
    Thread(F function, A argument);

    ////////////////////////////////////////////////////////////
    /// \brief Construct the thread from a member function and an object
    ///
    /// This constructor is template, which means that you can
    /// use it with any class.
    /// Use this constructor for this kind of function:
    /// \code
    /// class MyClass
    /// {
    /// public :
    ///
    ///     void function();
    /// };
    /// \endcode
    /// Note: this does *not* run the thread, use Run().
    ///
    /// \param function Entry point of the thread
    /// \param object Pointer to the object to use
    ///
    ////////////////////////////////////////////////////////////
    template <typename C>
    Thread(void(C::*function)(), C* object);

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    /// This destructor calls Wait(), so that the internal thread
    /// cannot survive after its sf::Thread instance is destroyed.
    ///
    ////////////////////////////////////////////////////////////
    ~Thread();

    ////////////////////////////////////////////////////////////
    /// \brief Run the thread
    ///
    /// This function starts the entry point passed to the
    /// thread's constructor, and returns immediately.
    /// After this function returns, the thread's function is
    /// running in parallel to the calling code.
    ///
    ////////////////////////////////////////////////////////////
    void Launch();

    ////////////////////////////////////////////////////////////
    /// \brief Wait until the thread finishes
    ///
    /// This function will block the execution until the
    /// thread's function ends.
    /// Warning: if the thread function never ends, the calling
    /// thread will block forever.
    ///
    ////////////////////////////////////////////////////////////
    void Wait();

    ////////////////////////////////////////////////////////////
    /// \brief Terminate the thread
    ///
    /// This function immediately stops the thread, without waiting
    /// for its function to finish.
    /// Terminating a thread with this function is not safe,
    /// and can lead to local variables not being destroyed
    /// on some operating systems. You should rather try to make
    /// the thread function terminate by itself.
    ///
    ////////////////////////////////////////////////////////////
    void Terminate();

private :

    friend class priv::ThreadImpl;

    ////////////////////////////////////////////////////////////
    /// \brief Internal entry point of the thread
    ///
    /// This function is called by the thread implementation.
    ///
    ////////////////////////////////////////////////////////////
    void Run();

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    priv::ThreadImpl* myImpl; ///< OS-specific implementation of the thread
    priv::ThreadFunc* myFunction; ///< Abstraction of the function to run
};

#include <SFML/System/Thread.inl>

} // namespace sf

#endif // SFML_THREAD_HPP


////////////////////////////////////////////////////////////
/// \class sf::Thread
/// \ingroup system
///
/// Threads provide a way to run multiple parts of the code
/// in parallel. When you launch a new thread, the execution
/// is split and both the new thread and the caller run
/// in parallel.
///
/// To use a sf::Thread, you construct it directly with the
/// function to execute as the entry point of the thread.
/// sf::Thread has multiple template constructors, which means
/// that you can use several types of entry points:
/// \li non-member functions with no argument
/// \li non-member functions with one argument of any type
/// \li functors with no argument (this one is particularly useful for compatibility with boost/std::bind)
/// \li functors with one argument of any type
/// \li member functions from any class with no argument
///
/// The function argument, if any, is copied in the sf::Thread
/// instance, as well as the functor (if the corresponding
/// constructor is used). Class instances, however, are passed
/// by pointer to you must make sure that the object won't be
/// destroyed while the thread is still using it.
///
/// The thread ends when its function is terminated. If the
/// owner sf::Thread instance is destroyed before the
/// thread is finished, the destructor will wait (see Wait())
///
/// Usage examples:
/// \code
/// // example 1: non member function with one argument
///
/// void ThreadFunc(int argument)
/// {
///     ...
/// }
///
/// sf::Thread thread(&ThreadFunc, 5);
/// thread.Launch(); // start the thread (internally calls ThreadFunc(5))
/// \endcode
///
/// \code
/// // example 2: member function
///
/// class Task
/// {
/// public :
///     void Run()
///     {
///         ...
///     }
/// };
///
/// Task task;
/// sf::Thread thread(&Task::Run, &task);
/// thread.Launch(); // start the thread (internally calls task.run())
/// \endcode
///
/// \code
/// // example 3: functor
///
/// struct Task
/// {
///     void operator()()
///     {
///         ...
///     }
/// };
///
/// sf::Thread thread(Task());
/// thread.Launch(); // start the thread (internally calls operator() on the Task instance)
/// \endcode
///
/// Creating parallel threads of execution can be dangerous:
/// all threads inside the same process share the same memory space,
/// which means that you may end up accessing the same variable
/// from multiple threads at the same time. To prevent this
/// kind of situations, you can use mutexes (see sf::Mutex).
///
/// \see sf::Mutex
///
////////////////////////////////////////////////////////////
