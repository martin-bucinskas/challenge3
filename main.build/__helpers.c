// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.h"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 1; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 1 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 1; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 1 * sizeof(PyObject *) );
            memcpy( python_pars + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 1 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 1 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 1; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 1 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 1 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 1 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 1, NULL );;
#else
                result = (*(_PyCFunctionFast)method)( self, &pos_args, 1 );;
#endif
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 1 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 2; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 2 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 2; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 2 * sizeof(PyObject *) );
            memcpy( python_pars + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 2 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 2 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 2; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 2 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 2 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 2 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 2, NULL );;
#else
                result = (*(_PyCFunctionFast)method)( self, &pos_args, 2 );;
#endif
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 2 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 3; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 3 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 3; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 3 * sizeof(PyObject *) );
            memcpy( python_pars + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 3 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 3 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 3; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 3 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 3 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 3 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 3, NULL );;
#else
                result = (*(_PyCFunctionFast)method)( self, &pos_args, 3 );;
#endif
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 3 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS4( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 4; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 4 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 4; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 4 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 4 * sizeof(PyObject *) );
            memcpy( python_pars + 4, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 4 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 4 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 4; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 4 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 4 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 4, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 4 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 4 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (4 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 4 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (4 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 4 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 4, NULL );;
#else
                result = (*(_PyCFunctionFast)method)( self, &pos_args, 4 );;
#endif
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            4
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 4 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS5( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 5; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 5 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 5; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 5 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 5 * sizeof(PyObject *) );
            memcpy( python_pars + 5, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 5 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 5 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 5; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 5 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 5 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 5, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 5 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 5 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (5 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 5 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (5 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_VARARGS )
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 5 );

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags & METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 5, NULL );;
#else
                result = (*(_PyCFunctionFast)method)( self, &pos_args, 5 );;
#endif
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            5
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 5 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_METHOD_WITH_ARGS1( PyObject *source, PyObject *attr_name, PyObject **args )
{
    CHECK_OBJECT( source );
    CHECK_OBJECT( attr_name );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 1; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    PyTypeObject *type = Py_TYPE( source );

    if ( type->tp_getattro == PyObject_GenericGetAttr )
    {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely( type->tp_dict == NULL ))
        {
            if (unlikely( PyType_Ready( type ) < 0 ))
            {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup( type, attr_name );
        descrgetfunc func = NULL;

        if ( descr != NULL )
        {
            Py_INCREF( descr );

#if PYTHON_VERSION < 300
            if ( PyType_HasFeature( Py_TYPE( descr ), Py_TPFLAGS_HAVE_CLASS ) )
            {
#endif
                func = Py_TYPE( descr )->tp_descr_get;

                if ( func != NULL && PyDescr_IsData( descr ) )
                {
                    PyObject *called_object = func( descr, source, (PyObject *)type );
                    Py_DECREF( descr );

                    PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                        called_object,
                        args
                    );
                    Py_DECREF( called_object );
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if ( dict != NULL )
        {
            CHECK_OBJECT( dict );

            Py_INCREF( dict );

            PyObject *called_object = PyDict_GetItem( dict, attr_name );

            if ( called_object != NULL )
            {
                Py_INCREF( called_object );
                Py_XDECREF( descr );
                Py_DECREF( dict );

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF( called_object );
                return result;
            }

            Py_DECREF( dict );
        }

        if ( func != NULL )
        {
            if ( func == Nuitka_Function_Type.tp_descr_get )
            {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    1
                );

                Py_DECREF( descr );

                return result;
            }
            else
            {
                PyObject *called_object = func( descr, source, (PyObject *)type );
                CHECK_OBJECT( called_object );

                Py_DECREF( descr );

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF( called_object );

                return result;
            }
        }

        if ( descr != NULL )
        {
            CHECK_OBJECT( descr );

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                descr,
                args
            );
            Py_DECREF( descr );

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if ( type == &PyInstance_Type )
    {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert( attr_name != const_str_plain___dict__ );
        assert( attr_name != const_str_plain___class__ );

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            return CALL_FUNCTION_WITH_ARGS1( called_object, args );
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            descrgetfunc descr_get = Py_TYPE( called_object )->tp_descr_get;

            if ( descr_get == Nuitka_Function_Type.tp_descr_get )
            {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    1
                );
            }
            else if ( descr_get != NULL )
            {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely( method == NULL ))
                {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS1( method, args );
                Py_DECREF( method );
                return result;
            }
            else
            {
                return CALL_FUNCTION_WITH_ARGS1( called_object, args );
            }

        }
        else if (unlikely( source_instance->in_class->cl_getattr == NULL ))
        {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        }
        else
        {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely( called_object == NULL ))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                called_object,
                args
            );
            Py_DECREF( called_object );
            return result;
        }
    }
#endif
    else if ( type->tp_getattro != NULL )
    {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else if ( type->tp_getattr != NULL )
    {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked( attr_name )
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else
    {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        return NULL;
    }
}

PyObject *CALL_METHOD_WITH_ARGS2( PyObject *source, PyObject *attr_name, PyObject **args )
{
    CHECK_OBJECT( source );
    CHECK_OBJECT( attr_name );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 2; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    PyTypeObject *type = Py_TYPE( source );

    if ( type->tp_getattro == PyObject_GenericGetAttr )
    {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely( type->tp_dict == NULL ))
        {
            if (unlikely( PyType_Ready( type ) < 0 ))
            {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup( type, attr_name );
        descrgetfunc func = NULL;

        if ( descr != NULL )
        {
            Py_INCREF( descr );

#if PYTHON_VERSION < 300
            if ( PyType_HasFeature( Py_TYPE( descr ), Py_TPFLAGS_HAVE_CLASS ) )
            {
#endif
                func = Py_TYPE( descr )->tp_descr_get;

                if ( func != NULL && PyDescr_IsData( descr ) )
                {
                    PyObject *called_object = func( descr, source, (PyObject *)type );
                    Py_DECREF( descr );

                    PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                        called_object,
                        args
                    );
                    Py_DECREF( called_object );
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if ( dict != NULL )
        {
            CHECK_OBJECT( dict );

            Py_INCREF( dict );

            PyObject *called_object = PyDict_GetItem( dict, attr_name );

            if ( called_object != NULL )
            {
                Py_INCREF( called_object );
                Py_XDECREF( descr );
                Py_DECREF( dict );

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                    called_object,
                    args
                );
                Py_DECREF( called_object );
                return result;
            }

            Py_DECREF( dict );
        }

        if ( func != NULL )
        {
            if ( func == Nuitka_Function_Type.tp_descr_get )
            {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    2
                );

                Py_DECREF( descr );

                return result;
            }
            else
            {
                PyObject *called_object = func( descr, source, (PyObject *)type );
                CHECK_OBJECT( called_object );

                Py_DECREF( descr );

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                    called_object,
                    args
                );
                Py_DECREF( called_object );

                return result;
            }
        }

        if ( descr != NULL )
        {
            CHECK_OBJECT( descr );

            PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                descr,
                args
            );
            Py_DECREF( descr );

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if ( type == &PyInstance_Type )
    {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert( attr_name != const_str_plain___dict__ );
        assert( attr_name != const_str_plain___class__ );

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            return CALL_FUNCTION_WITH_ARGS2( called_object, args );
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            descrgetfunc descr_get = Py_TYPE( called_object )->tp_descr_get;

            if ( descr_get == Nuitka_Function_Type.tp_descr_get )
            {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    2
                );
            }
            else if ( descr_get != NULL )
            {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely( method == NULL ))
                {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS2( method, args );
                Py_DECREF( method );
                return result;
            }
            else
            {
                return CALL_FUNCTION_WITH_ARGS2( called_object, args );
            }

        }
        else if (unlikely( source_instance->in_class->cl_getattr == NULL ))
        {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        }
        else
        {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely( called_object == NULL ))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                called_object,
                args
            );
            Py_DECREF( called_object );
            return result;
        }
    }
#endif
    else if ( type->tp_getattro != NULL )
    {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS2(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else if ( type->tp_getattr != NULL )
    {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked( attr_name )
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS2(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else
    {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        return NULL;
    }
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.h"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
MOD_ENTRY_DECL(City);
MOD_INIT_DECL(__main__);
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] =
{
    { "City", MOD_INIT_NAME( City ), 0, 0,  },
    { "__main__", MOD_INIT_NAME( __main__ ), 0, 0,  },
    { "_asyncio", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_bisect", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_blake2", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_bz2", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_codecs_cn", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_codecs_hk", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_codecs_iso2022", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_codecs_jp", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_codecs_kr", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_codecs_tw", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_contextvars", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_crypt", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_csv", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_ctypes", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_curses", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_curses_panel", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_datetime", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_dbm", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_decimal", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_elementtree", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_hashlib", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_heapq", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_json", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_lsprof", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_lzma", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_multibytecodec", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_multiprocessing", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_opcode", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_pickle", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_posixsubprocess", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_queue", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_random", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_scproxy", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_sha3", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_socket", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_sqlite3", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_ssl", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_struct", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_tkinter", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "_uuid", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "array", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "audioop", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "binascii", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "csv", NULL, 1843, 11843, NUITKA_BYTECODE_FLAG },
    { "fcntl", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "grp", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "json", NULL, 13686, 12349, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "math", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "mmap", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "pyexpat", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "readline", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "select", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "site", NULL, 26035, 13161, NUITKA_BYTECODE_FLAG },
    { "termios", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "timeit", NULL, 39196, 11658, NUITKA_BYTECODE_FLAG },
    { "unicodedata", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "zlib", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { "__future__", NULL, 50854, 4143, NUITKA_BYTECODE_FLAG },
    { "_bootlocale", NULL, 54997, 1260, NUITKA_BYTECODE_FLAG },
    { "_compat_pickle", NULL, 56257, 5819, NUITKA_BYTECODE_FLAG },
    { "_dummy_thread", NULL, 62076, 6000, NUITKA_BYTECODE_FLAG },
    { "_markupbase", NULL, 68076, 7796, NUITKA_BYTECODE_FLAG },
    { "_osx_support", NULL, 75872, 9606, NUITKA_BYTECODE_FLAG },
    { "_py_abc", NULL, 85478, 4665, NUITKA_BYTECODE_FLAG },
    { "_pyio", NULL, 90143, 72879, NUITKA_BYTECODE_FLAG },
    { "_sitebuiltins", NULL, 163022, 3476, NUITKA_BYTECODE_FLAG },
    { "_strptime", NULL, 166498, 16115, NUITKA_BYTECODE_FLAG },
    { "_sysconfigdata_m_darwin_darwin", NULL, 182613, 19833, NUITKA_BYTECODE_FLAG },
    { "_threading_local", NULL, 202446, 6423, NUITKA_BYTECODE_FLAG },
    { "aifc", NULL, 208869, 26154, NUITKA_BYTECODE_FLAG },
    { "argparse", NULL, 235023, 61924, NUITKA_BYTECODE_FLAG },
    { "ast", NULL, 296947, 12091, NUITKA_BYTECODE_FLAG },
    { "asynchat", NULL, 309038, 6845, NUITKA_BYTECODE_FLAG },
    { "asyncio", NULL, 315883, 701, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "asyncio.base_events", NULL, 316584, 48117, NUITKA_BYTECODE_FLAG },
    { "asyncio.base_futures", NULL, 364701, 2113, NUITKA_BYTECODE_FLAG },
    { "asyncio.base_subprocess", NULL, 366814, 9202, NUITKA_BYTECODE_FLAG },
    { "asyncio.base_tasks", NULL, 376016, 1877, NUITKA_BYTECODE_FLAG },
    { "asyncio.constants", NULL, 377893, 602, NUITKA_BYTECODE_FLAG },
    { "asyncio.coroutines", NULL, 378495, 6389, NUITKA_BYTECODE_FLAG },
    { "asyncio.events", NULL, 384884, 27866, NUITKA_BYTECODE_FLAG },
    { "asyncio.format_helpers", NULL, 412750, 2328, NUITKA_BYTECODE_FLAG },
    { "asyncio.futures", NULL, 415078, 10756, NUITKA_BYTECODE_FLAG },
    { "asyncio.locks", NULL, 425834, 15924, NUITKA_BYTECODE_FLAG },
    { "asyncio.log", NULL, 441758, 251, NUITKA_BYTECODE_FLAG },
    { "asyncio.proactor_events", NULL, 442009, 20099, NUITKA_BYTECODE_FLAG },
    { "asyncio.protocols", NULL, 462108, 8739, NUITKA_BYTECODE_FLAG },
    { "asyncio.queues", NULL, 470847, 8184, NUITKA_BYTECODE_FLAG },
    { "asyncio.runners", NULL, 479031, 1925, NUITKA_BYTECODE_FLAG },
    { "asyncio.selector_events", NULL, 480956, 28191, NUITKA_BYTECODE_FLAG },
    { "asyncio.sslproto", NULL, 509147, 21270, NUITKA_BYTECODE_FLAG },
    { "asyncio.streams", NULL, 530417, 20299, NUITKA_BYTECODE_FLAG },
    { "asyncio.subprocess", NULL, 550716, 6763, NUITKA_BYTECODE_FLAG },
    { "asyncio.tasks", NULL, 557479, 22330, NUITKA_BYTECODE_FLAG },
    { "asyncio.transports", NULL, 579809, 12222, NUITKA_BYTECODE_FLAG },
    { "asyncio.unix_events", NULL, 592031, 32140, NUITKA_BYTECODE_FLAG },
    { "asyncore", NULL, 624171, 15855, NUITKA_BYTECODE_FLAG },
    { "bdb", NULL, 640026, 24658, NUITKA_BYTECODE_FLAG },
    { "binhex", NULL, 664684, 12070, NUITKA_BYTECODE_FLAG },
    { "bisect", NULL, 676754, 2709, NUITKA_BYTECODE_FLAG },
    { "cProfile", NULL, 679463, 4819, NUITKA_BYTECODE_FLAG },
    { "calendar", NULL, 684282, 27435, NUITKA_BYTECODE_FLAG },
    { "cgi", NULL, 711717, 27202, NUITKA_BYTECODE_FLAG },
    { "cgitb", NULL, 738919, 10123, NUITKA_BYTECODE_FLAG },
    { "chunk", NULL, 749042, 4930, NUITKA_BYTECODE_FLAG },
    { "cmd", NULL, 753972, 12601, NUITKA_BYTECODE_FLAG },
    { "code", NULL, 766573, 9869, NUITKA_BYTECODE_FLAG },
    { "codeop", NULL, 776442, 6301, NUITKA_BYTECODE_FLAG },
    { "colorsys", NULL, 782743, 3308, NUITKA_BYTECODE_FLAG },
    { "compileall", NULL, 786051, 9117, NUITKA_BYTECODE_FLAG },
    { "concurrent", NULL, 795168, 157, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "concurrent.futures", NULL, 795325, 1097, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "concurrent.futures._base", NULL, 796422, 21195, NUITKA_BYTECODE_FLAG },
    { "concurrent.futures.process", NULL, 817617, 20004, NUITKA_BYTECODE_FLAG },
    { "concurrent.futures.thread", NULL, 837621, 5444, NUITKA_BYTECODE_FLAG },
    { "configparser", NULL, 843065, 45891, NUITKA_BYTECODE_FLAG },
    { "contextlib", NULL, 888956, 20469, NUITKA_BYTECODE_FLAG },
    { "contextvars", NULL, 909425, 268, NUITKA_BYTECODE_FLAG },
    { "copy", NULL, 909693, 7112, NUITKA_BYTECODE_FLAG },
    { "crypt", NULL, 916805, 3145, NUITKA_BYTECODE_FLAG },
    { "csv", NULL, 1843, 11843, NUITKA_BYTECODE_FLAG },
    { "ctypes", NULL, 919950, 16390, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "ctypes._aix", NULL, 936340, 9834, NUITKA_BYTECODE_FLAG },
    { "ctypes._endian", NULL, 946174, 1967, NUITKA_BYTECODE_FLAG },
    { "ctypes.macholib", NULL, 948141, 324, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "ctypes.macholib.dyld", NULL, 948465, 4357, NUITKA_BYTECODE_FLAG },
    { "ctypes.macholib.dylib", NULL, 952822, 1951, NUITKA_BYTECODE_FLAG },
    { "ctypes.macholib.framework", NULL, 954773, 2231, NUITKA_BYTECODE_FLAG },
    { "ctypes.util", NULL, 957004, 7760, NUITKA_BYTECODE_FLAG },
    { "curses", NULL, 964764, 1951, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "curses.ascii", NULL, 966715, 3914, NUITKA_BYTECODE_FLAG },
    { "curses.has_key", NULL, 970629, 4278, NUITKA_BYTECODE_FLAG },
    { "curses.panel", NULL, 974907, 248, NUITKA_BYTECODE_FLAG },
    { "curses.textpad", NULL, 975155, 5917, NUITKA_BYTECODE_FLAG },
    { "dataclasses", NULL, 981072, 22476, NUITKA_BYTECODE_FLAG },
    { "datetime", NULL, 1003548, 57260, NUITKA_BYTECODE_FLAG },
    { "dbm", NULL, 1060808, 4177, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "dbm.dumb", NULL, 1064985, 8411, NUITKA_BYTECODE_FLAG },
    { "dbm.ndbm", NULL, 1073396, 227, NUITKA_BYTECODE_FLAG },
    { "decimal", NULL, 1073623, 162183, NUITKA_BYTECODE_FLAG },
    { "difflib", NULL, 1235806, 59449, NUITKA_BYTECODE_FLAG },
    { "distutils", NULL, 1295255, 409, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "distutils.archive_util", NULL, 1295664, 6543, NUITKA_BYTECODE_FLAG },
    { "distutils.bcppcompiler", NULL, 1302207, 6512, NUITKA_BYTECODE_FLAG },
    { "distutils.ccompiler", NULL, 1308719, 33227, NUITKA_BYTECODE_FLAG },
    { "distutils.cmd", NULL, 1341946, 13919, NUITKA_BYTECODE_FLAG },
    { "distutils.command", NULL, 1355865, 566, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "distutils.command.bdist", NULL, 1356431, 3665, NUITKA_BYTECODE_FLAG },
    { "distutils.command.bdist_dumb", NULL, 1360096, 3580, NUITKA_BYTECODE_FLAG },
    { "distutils.command.bdist_rpm", NULL, 1363676, 12505, NUITKA_BYTECODE_FLAG },
    { "distutils.command.bdist_wininst", NULL, 1376181, 8020, NUITKA_BYTECODE_FLAG },
    { "distutils.command.build", NULL, 1384201, 3854, NUITKA_BYTECODE_FLAG },
    { "distutils.command.build_clib", NULL, 1388055, 4896, NUITKA_BYTECODE_FLAG },
    { "distutils.command.build_ext", NULL, 1392951, 15803, NUITKA_BYTECODE_FLAG },
    { "distutils.command.build_py", NULL, 1408754, 10402, NUITKA_BYTECODE_FLAG },
    { "distutils.command.build_scripts", NULL, 1419156, 4301, NUITKA_BYTECODE_FLAG },
    { "distutils.command.check", NULL, 1423457, 4850, NUITKA_BYTECODE_FLAG },
    { "distutils.command.clean", NULL, 1428307, 2103, NUITKA_BYTECODE_FLAG },
    { "distutils.command.config", NULL, 1430410, 10207, NUITKA_BYTECODE_FLAG },
    { "distutils.command.install", NULL, 1440617, 13530, NUITKA_BYTECODE_FLAG },
    { "distutils.command.install_data", NULL, 1454147, 2296, NUITKA_BYTECODE_FLAG },
    { "distutils.command.install_egg_info", NULL, 1456443, 2994, NUITKA_BYTECODE_FLAG },
    { "distutils.command.install_headers", NULL, 1459437, 1711, NUITKA_BYTECODE_FLAG },
    { "distutils.command.install_lib", NULL, 1461148, 5089, NUITKA_BYTECODE_FLAG },
    { "distutils.command.install_scripts", NULL, 1466237, 2153, NUITKA_BYTECODE_FLAG },
    { "distutils.command.register", NULL, 1468390, 8488, NUITKA_BYTECODE_FLAG },
    { "distutils.command.sdist", NULL, 1476878, 14519, NUITKA_BYTECODE_FLAG },
    { "distutils.command.upload", NULL, 1491397, 5101, NUITKA_BYTECODE_FLAG },
    { "distutils.config", NULL, 1496498, 3496, NUITKA_BYTECODE_FLAG },
    { "distutils.core", NULL, 1499994, 6621, NUITKA_BYTECODE_FLAG },
    { "distutils.cygwinccompiler", NULL, 1506615, 8539, NUITKA_BYTECODE_FLAG },
    { "distutils.debug", NULL, 1515154, 219, NUITKA_BYTECODE_FLAG },
    { "distutils.dep_util", NULL, 1515373, 2735, NUITKA_BYTECODE_FLAG },
    { "distutils.dir_util", NULL, 1518108, 5829, NUITKA_BYTECODE_FLAG },
    { "distutils.dist", NULL, 1523937, 34450, NUITKA_BYTECODE_FLAG },
    { "distutils.errors", NULL, 1558387, 5505, NUITKA_BYTECODE_FLAG },
    { "distutils.extension", NULL, 1563892, 6916, NUITKA_BYTECODE_FLAG },
    { "distutils.fancy_getopt", NULL, 1570808, 10628, NUITKA_BYTECODE_FLAG },
    { "distutils.file_util", NULL, 1581436, 5914, NUITKA_BYTECODE_FLAG },
    { "distutils.filelist", NULL, 1587350, 9849, NUITKA_BYTECODE_FLAG },
    { "distutils.log", NULL, 1597199, 2330, NUITKA_BYTECODE_FLAG },
    { "distutils.msvccompiler", NULL, 1599529, 14582, NUITKA_BYTECODE_FLAG },
    { "distutils.spawn", NULL, 1614111, 5133, NUITKA_BYTECODE_FLAG },
    { "distutils.sysconfig", NULL, 1619244, 12007, NUITKA_BYTECODE_FLAG },
    { "distutils.text_file", NULL, 1631251, 8456, NUITKA_BYTECODE_FLAG },
    { "distutils.unixccompiler", NULL, 1639707, 6551, NUITKA_BYTECODE_FLAG },
    { "distutils.util", NULL, 1646258, 15112, NUITKA_BYTECODE_FLAG },
    { "distutils.version", NULL, 1661370, 7367, NUITKA_BYTECODE_FLAG },
    { "distutils.versionpredicate", NULL, 1668737, 5114, NUITKA_BYTECODE_FLAG },
    { "doctest", NULL, 1673851, 75445, NUITKA_BYTECODE_FLAG },
    { "dummy_threading", NULL, 1749296, 1135, NUITKA_BYTECODE_FLAG },
    { "email", NULL, 1750431, 1702, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "email._encoded_words", NULL, 1752133, 5619, NUITKA_BYTECODE_FLAG },
    { "email._header_value_parser", NULL, 1757752, 76154, NUITKA_BYTECODE_FLAG },
    { "email._parseaddr", NULL, 1833906, 12359, NUITKA_BYTECODE_FLAG },
    { "email._policybase", NULL, 1846265, 14861, NUITKA_BYTECODE_FLAG },
    { "email.base64mime", NULL, 1861126, 3246, NUITKA_BYTECODE_FLAG },
    { "email.charset", NULL, 1864372, 11540, NUITKA_BYTECODE_FLAG },
    { "email.contentmanager", NULL, 1875912, 7306, NUITKA_BYTECODE_FLAG },
    { "email.encoders", NULL, 1883218, 1675, NUITKA_BYTECODE_FLAG },
    { "email.errors", NULL, 1884893, 6202, NUITKA_BYTECODE_FLAG },
    { "email.feedparser", NULL, 1891095, 10640, NUITKA_BYTECODE_FLAG },
    { "email.generator", NULL, 1901735, 12511, NUITKA_BYTECODE_FLAG },
    { "email.header", NULL, 1914246, 16397, NUITKA_BYTECODE_FLAG },
    { "email.headerregistry", NULL, 1930643, 21308, NUITKA_BYTECODE_FLAG },
    { "email.iterators", NULL, 1951951, 1943, NUITKA_BYTECODE_FLAG },
    { "email.message", NULL, 1953894, 37951, NUITKA_BYTECODE_FLAG },
    { "email.mime", NULL, 1991845, 157, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "email.mime.application", NULL, 1992002, 1468, NUITKA_BYTECODE_FLAG },
    { "email.mime.audio", NULL, 1993470, 2627, NUITKA_BYTECODE_FLAG },
    { "email.mime.base", NULL, 1996097, 1093, NUITKA_BYTECODE_FLAG },
    { "email.mime.image", NULL, 1997190, 1913, NUITKA_BYTECODE_FLAG },
    { "email.mime.message", NULL, 1999103, 1342, NUITKA_BYTECODE_FLAG },
    { "email.mime.multipart", NULL, 2000445, 1564, NUITKA_BYTECODE_FLAG },
    { "email.mime.nonmultipart", NULL, 2002009, 779, NUITKA_BYTECODE_FLAG },
    { "email.mime.text", NULL, 2002788, 1326, NUITKA_BYTECODE_FLAG },
    { "email.parser", NULL, 2004114, 5758, NUITKA_BYTECODE_FLAG },
    { "email.policy", NULL, 2009872, 9666, NUITKA_BYTECODE_FLAG },
    { "email.quoprimime", NULL, 2019538, 7675, NUITKA_BYTECODE_FLAG },
    { "email.utils", NULL, 2027213, 9478, NUITKA_BYTECODE_FLAG },
    { "filecmp", NULL, 2036691, 8318, NUITKA_BYTECODE_FLAG },
    { "fileinput", NULL, 2045009, 13266, NUITKA_BYTECODE_FLAG },
    { "fnmatch", NULL, 2058275, 3348, NUITKA_BYTECODE_FLAG },
    { "formatter", NULL, 2061623, 17564, NUITKA_BYTECODE_FLAG },
    { "fractions", NULL, 2079187, 18439, NUITKA_BYTECODE_FLAG },
    { "ftplib", NULL, 2097626, 28078, NUITKA_BYTECODE_FLAG },
    { "getopt", NULL, 2125704, 6250, NUITKA_BYTECODE_FLAG },
    { "getpass", NULL, 2131954, 4175, NUITKA_BYTECODE_FLAG },
    { "gettext", NULL, 2136129, 14179, NUITKA_BYTECODE_FLAG },
    { "glob", NULL, 2150308, 4270, NUITKA_BYTECODE_FLAG },
    { "gzip", NULL, 2154578, 17196, NUITKA_BYTECODE_FLAG },
    { "hashlib", NULL, 2171774, 6602, NUITKA_BYTECODE_FLAG },
    { "hmac", NULL, 2178376, 6124, NUITKA_BYTECODE_FLAG },
    { "html", NULL, 2184500, 3408, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "html.entities", NULL, 2187908, 50480, NUITKA_BYTECODE_FLAG },
    { "html.parser", NULL, 2238388, 11118, NUITKA_BYTECODE_FLAG },
    { "http", NULL, 2249506, 5933, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "http.client", NULL, 2255439, 34327, NUITKA_BYTECODE_FLAG },
    { "http.cookiejar", NULL, 2289766, 53596, NUITKA_BYTECODE_FLAG },
    { "http.cookies", NULL, 2343362, 15255, NUITKA_BYTECODE_FLAG },
    { "http.server", NULL, 2358617, 33378, NUITKA_BYTECODE_FLAG },
    { "imaplib", NULL, 2391995, 41441, NUITKA_BYTECODE_FLAG },
    { "imghdr", NULL, 2433436, 4153, NUITKA_BYTECODE_FLAG },
    { "imp", NULL, 2437589, 9764, NUITKA_BYTECODE_FLAG },
    { "importlib.abc", NULL, 2447353, 13498, NUITKA_BYTECODE_FLAG },
    { "importlib.resources", NULL, 2460851, 8350, NUITKA_BYTECODE_FLAG },
    { "importlib.util", NULL, 2469201, 9367, NUITKA_BYTECODE_FLAG },
    { "ipaddress", NULL, 2478568, 63111, NUITKA_BYTECODE_FLAG },
    { "json", NULL, 13686, 12349, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "json.decoder", NULL, 2541679, 9835, NUITKA_BYTECODE_FLAG },
    { "json.encoder", NULL, 2551514, 11317, NUITKA_BYTECODE_FLAG },
    { "json.scanner", NULL, 2562831, 2007, NUITKA_BYTECODE_FLAG },
    { "json.tool", NULL, 2564838, 1488, NUITKA_BYTECODE_FLAG },
    { "lib2to3", NULL, 2566326, 154, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "lib2to3.btm_matcher", NULL, 2566480, 4904, NUITKA_BYTECODE_FLAG },
    { "lib2to3.btm_utils", NULL, 2571384, 6154, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixer_base", NULL, 2577538, 6246, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixer_util", NULL, 2583784, 12057, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes", NULL, 2595841, 160, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "lib2to3.fixes.fix_apply", NULL, 2596001, 1698, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_asserts", NULL, 2597699, 1283, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_basestring", NULL, 2598982, 673, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_buffer", NULL, 2599655, 818, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_dict", NULL, 2600473, 3326, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_except", NULL, 2603799, 2828, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_exec", NULL, 2606627, 1159, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_execfile", NULL, 2607786, 1703, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_exitfunc", NULL, 2609489, 2306, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_filter", NULL, 2611795, 2371, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_funcattrs", NULL, 2614166, 984, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_future", NULL, 2615150, 794, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_getcwdu", NULL, 2615944, 798, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_has_key", NULL, 2616742, 2928, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_idioms", NULL, 2619670, 3913, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_import", NULL, 2623583, 2795, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_imports", NULL, 2626378, 4359, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_imports2", NULL, 2630737, 558, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_input", NULL, 2631295, 960, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_intern", NULL, 2632255, 1167, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_isinstance", NULL, 2633422, 1565, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_itertools", NULL, 2634987, 1554, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_itertools_imports", NULL, 2636541, 1590, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_long", NULL, 2638131, 715, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_map", NULL, 2638846, 3103, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_metaclass", NULL, 2641949, 5356, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_methodattrs", NULL, 2647305, 946, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_ne", NULL, 2648251, 817, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_next", NULL, 2649068, 3070, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_nonzero", NULL, 2652138, 933, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_numliterals", NULL, 2653071, 1029, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_operator", NULL, 2654100, 4246, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_paren", NULL, 2658346, 1400, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_print", NULL, 2659746, 2335, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_raise", NULL, 2662081, 2259, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_raw_input", NULL, 2664340, 805, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_reduce", NULL, 2665145, 1140, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_reload", NULL, 2666285, 1179, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_renames", NULL, 2667464, 2003, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_repr", NULL, 2669467, 855, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_set_literal", NULL, 2670322, 1687, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_standarderror", NULL, 2672009, 730, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_sys_exc", NULL, 2672739, 1411, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_throw", NULL, 2674150, 1812, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_tuple_params", NULL, 2675962, 4606, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_types", NULL, 2680568, 1839, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_unicode", NULL, 2682407, 1551, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_urllib", NULL, 2683958, 5971, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_ws_comma", NULL, 2689929, 1133, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_xrange", NULL, 2691062, 2546, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_xreadlines", NULL, 2693608, 1127, NUITKA_BYTECODE_FLAG },
    { "lib2to3.fixes.fix_zip", NULL, 2694735, 1591, NUITKA_BYTECODE_FLAG },
    { "lib2to3.main", NULL, 2696326, 8547, NUITKA_BYTECODE_FLAG },
    { "lib2to3.patcomp", NULL, 2704873, 5622, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pgen2", NULL, 2710495, 190, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "lib2to3.pgen2.driver", NULL, 2710685, 5151, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pgen2.grammar", NULL, 2715836, 7027, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pgen2.literals", NULL, 2722863, 1569, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pgen2.parse", NULL, 2724432, 6315, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pgen2.pgen", NULL, 2730747, 9791, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pgen2.token", NULL, 2740538, 1883, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pgen2.tokenize", NULL, 2742421, 15150, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pygram", NULL, 2757571, 1209, NUITKA_BYTECODE_FLAG },
    { "lib2to3.pytree", NULL, 2758780, 25014, NUITKA_BYTECODE_FLAG },
    { "lib2to3.refactor", NULL, 2783794, 20415, NUITKA_BYTECODE_FLAG },
    { "logging", NULL, 2804209, 62413, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "logging.config", NULL, 2866622, 23026, NUITKA_BYTECODE_FLAG },
    { "logging.handlers", NULL, 2889648, 43043, NUITKA_BYTECODE_FLAG },
    { "lzma", NULL, 2932691, 11950, NUITKA_BYTECODE_FLAG },
    { "macpath", NULL, 2944641, 5818, NUITKA_BYTECODE_FLAG },
    { "mailbox", NULL, 2950459, 63659, NUITKA_BYTECODE_FLAG },
    { "mailcap", NULL, 3014118, 6495, NUITKA_BYTECODE_FLAG },
    { "mimetypes", NULL, 3020613, 15489, NUITKA_BYTECODE_FLAG },
    { "modulefinder", NULL, 3036102, 15363, NUITKA_BYTECODE_FLAG },
    { "multiprocessing", NULL, 3051465, 538, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "multiprocessing.connection", NULL, 3052003, 24943, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.context", NULL, 3076946, 13124, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.dummy", NULL, 3090070, 3816, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "multiprocessing.dummy.connection", NULL, 3093886, 2524, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.forkserver", NULL, 3096410, 7775, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.heap", NULL, 3104185, 6435, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.managers", NULL, 3110620, 34376, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.pool", NULL, 3144996, 21247, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.popen_fork", NULL, 3166243, 2537, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.popen_forkserver", NULL, 3168780, 2370, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.popen_spawn_posix", NULL, 3171150, 2152, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.process", NULL, 3173302, 9437, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.queues", NULL, 3182739, 9448, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.reduction", NULL, 3192187, 8029, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.resource_sharer", NULL, 3200216, 5215, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.semaphore_tracker", NULL, 3205431, 3751, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.sharedctypes", NULL, 3209182, 6928, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.spawn", NULL, 3216110, 6479, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.synchronize", NULL, 3222589, 11194, NUITKA_BYTECODE_FLAG },
    { "multiprocessing.util", NULL, 3233783, 9958, NUITKA_BYTECODE_FLAG },
    { "netrc", NULL, 3243741, 3774, NUITKA_BYTECODE_FLAG },
    { "nntplib", NULL, 3247515, 33762, NUITKA_BYTECODE_FLAG },
    { "ntpath", NULL, 3281277, 13015, NUITKA_BYTECODE_FLAG },
    { "nturl2path", NULL, 3294292, 1626, NUITKA_BYTECODE_FLAG },
    { "numbers", NULL, 3295918, 12203, NUITKA_BYTECODE_FLAG },
    { "optparse", NULL, 3308121, 47904, NUITKA_BYTECODE_FLAG },
    { "pathlib", NULL, 3356025, 41757, NUITKA_BYTECODE_FLAG },
    { "pdb", NULL, 3397782, 46939, NUITKA_BYTECODE_FLAG },
    { "pickle", NULL, 3444721, 43052, NUITKA_BYTECODE_FLAG },
    { "pickletools", NULL, 3487773, 65342, NUITKA_BYTECODE_FLAG },
    { "pipes", NULL, 3553115, 7814, NUITKA_BYTECODE_FLAG },
    { "pkgutil", NULL, 3560929, 16371, NUITKA_BYTECODE_FLAG },
    { "platform", NULL, 3577300, 28175, NUITKA_BYTECODE_FLAG },
    { "plistlib", NULL, 3605475, 25106, NUITKA_BYTECODE_FLAG },
    { "poplib", NULL, 3630581, 13347, NUITKA_BYTECODE_FLAG },
    { "pprint", NULL, 3643928, 15844, NUITKA_BYTECODE_FLAG },
    { "profile", NULL, 3659772, 14103, NUITKA_BYTECODE_FLAG },
    { "pstats", NULL, 3673875, 22305, NUITKA_BYTECODE_FLAG },
    { "pty", NULL, 3696180, 3894, NUITKA_BYTECODE_FLAG },
    { "py_compile", NULL, 3700074, 7202, NUITKA_BYTECODE_FLAG },
    { "pyclbr", NULL, 3707276, 10384, NUITKA_BYTECODE_FLAG },
    { "pydoc", NULL, 3717660, 84441, NUITKA_BYTECODE_FLAG },
    { "pydoc_data", NULL, 3802101, 157, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "pydoc_data.topics", NULL, 3802258, 411743, NUITKA_BYTECODE_FLAG },
    { "queue", NULL, 4214001, 11483, NUITKA_BYTECODE_FLAG },
    { "random", NULL, 4225484, 19419, NUITKA_BYTECODE_FLAG },
    { "rlcompleter", NULL, 4244903, 5758, NUITKA_BYTECODE_FLAG },
    { "runpy", NULL, 4250661, 7956, NUITKA_BYTECODE_FLAG },
    { "sched", NULL, 4258617, 6532, NUITKA_BYTECODE_FLAG },
    { "secrets", NULL, 4265149, 2195, NUITKA_BYTECODE_FLAG },
    { "selectors", NULL, 4267344, 16959, NUITKA_BYTECODE_FLAG },
    { "shelve", NULL, 4284303, 9517, NUITKA_BYTECODE_FLAG },
    { "shlex", NULL, 4293820, 7001, NUITKA_BYTECODE_FLAG },
    { "shutil", NULL, 4300821, 30991, NUITKA_BYTECODE_FLAG },
    { "signal", NULL, 4331812, 2523, NUITKA_BYTECODE_FLAG },
    { "site", NULL, 26035, 13161, NUITKA_BYTECODE_FLAG },
    { "smtpd", NULL, 4334335, 26615, NUITKA_BYTECODE_FLAG },
    { "smtplib", NULL, 4360950, 35361, NUITKA_BYTECODE_FLAG },
    { "sndhdr", NULL, 4396311, 6914, NUITKA_BYTECODE_FLAG },
    { "socket", NULL, 4403225, 22032, NUITKA_BYTECODE_FLAG },
    { "socketserver", NULL, 4425257, 24210, NUITKA_BYTECODE_FLAG },
    { "sqlite3", NULL, 4449467, 185, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "sqlite3.dbapi2", NULL, 4449652, 2504, NUITKA_BYTECODE_FLAG },
    { "sqlite3.dump", NULL, 4452156, 1947, NUITKA_BYTECODE_FLAG },
    { "ssl", NULL, 4454103, 39401, NUITKA_BYTECODE_FLAG },
    { "statistics", NULL, 4493504, 18175, NUITKA_BYTECODE_FLAG },
    { "string", NULL, 4511679, 7846, NUITKA_BYTECODE_FLAG },
    { "subprocess", NULL, 4519525, 38799, NUITKA_BYTECODE_FLAG },
    { "sunau", NULL, 4558324, 17222, NUITKA_BYTECODE_FLAG },
    { "symbol", NULL, 4575546, 2576, NUITKA_BYTECODE_FLAG },
    { "symtable", NULL, 4578122, 10456, NUITKA_BYTECODE_FLAG },
    { "sysconfig", NULL, 4588578, 15455, NUITKA_BYTECODE_FLAG },
    { "tabnanny", NULL, 4604033, 6989, NUITKA_BYTECODE_FLAG },
    { "tarfile", NULL, 4611022, 61852, NUITKA_BYTECODE_FLAG },
    { "telnetlib", NULL, 4672874, 18113, NUITKA_BYTECODE_FLAG },
    { "tempfile", NULL, 4690987, 22156, NUITKA_BYTECODE_FLAG },
    { "textwrap", NULL, 4713143, 13623, NUITKA_BYTECODE_FLAG },
    { "this", NULL, 4726766, 1288, NUITKA_BYTECODE_FLAG },
    { "timeit", NULL, 39196, 11658, NUITKA_BYTECODE_FLAG },
    { "tkinter", NULL, 4728054, 179186, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "tkinter.colorchooser", NULL, 4907240, 1146, NUITKA_BYTECODE_FLAG },
    { "tkinter.commondialog", NULL, 4908386, 1127, NUITKA_BYTECODE_FLAG },
    { "tkinter.constants", NULL, 4909513, 1679, NUITKA_BYTECODE_FLAG },
    { "tkinter.dialog", NULL, 4911192, 1455, NUITKA_BYTECODE_FLAG },
    { "tkinter.dnd", NULL, 4912647, 11197, NUITKA_BYTECODE_FLAG },
    { "tkinter.filedialog", NULL, 4923844, 12281, NUITKA_BYTECODE_FLAG },
    { "tkinter.font", NULL, 4936125, 6198, NUITKA_BYTECODE_FLAG },
    { "tkinter.messagebox", NULL, 4942323, 3063, NUITKA_BYTECODE_FLAG },
    { "tkinter.scrolledtext", NULL, 4945386, 2190, NUITKA_BYTECODE_FLAG },
    { "tkinter.simpledialog", NULL, 4947576, 10557, NUITKA_BYTECODE_FLAG },
    { "tkinter.tix", NULL, 4958133, 83677, NUITKA_BYTECODE_FLAG },
    { "tkinter.ttk", NULL, 5041810, 57861, NUITKA_BYTECODE_FLAG },
    { "trace", NULL, 5099671, 19413, NUITKA_BYTECODE_FLAG },
    { "tracemalloc", NULL, 5119084, 17287, NUITKA_BYTECODE_FLAG },
    { "tty", NULL, 5136371, 1105, NUITKA_BYTECODE_FLAG },
    { "turtle", NULL, 5137476, 130802, NUITKA_BYTECODE_FLAG },
    { "typing", NULL, 5268278, 49991, NUITKA_BYTECODE_FLAG },
    { "unittest", NULL, 5318269, 3022, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "unittest.case", NULL, 5321291, 48369, NUITKA_BYTECODE_FLAG },
    { "unittest.loader", NULL, 5369660, 14287, NUITKA_BYTECODE_FLAG },
    { "unittest.main", NULL, 5383947, 7449, NUITKA_BYTECODE_FLAG },
    { "unittest.mock", NULL, 5391396, 63433, NUITKA_BYTECODE_FLAG },
    { "unittest.result", NULL, 5454829, 7263, NUITKA_BYTECODE_FLAG },
    { "unittest.runner", NULL, 5462092, 7006, NUITKA_BYTECODE_FLAG },
    { "unittest.signals", NULL, 5469098, 2205, NUITKA_BYTECODE_FLAG },
    { "unittest.suite", NULL, 5471303, 9214, NUITKA_BYTECODE_FLAG },
    { "unittest.util", NULL, 5480517, 4532, NUITKA_BYTECODE_FLAG },
    { "urllib", NULL, 5485049, 153, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "urllib.error", NULL, 5485202, 2787, NUITKA_BYTECODE_FLAG },
    { "urllib.parse", NULL, 5487989, 30844, NUITKA_BYTECODE_FLAG },
    { "urllib.request", NULL, 5518833, 72330, NUITKA_BYTECODE_FLAG },
    { "urllib.response", NULL, 5591163, 3260, NUITKA_BYTECODE_FLAG },
    { "urllib.robotparser", NULL, 5594423, 7100, NUITKA_BYTECODE_FLAG },
    { "uu", NULL, 5601523, 3583, NUITKA_BYTECODE_FLAG },
    { "uuid", NULL, 5605106, 23210, NUITKA_BYTECODE_FLAG },
    { "venv", NULL, 5628316, 14466, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "wave", NULL, 5642782, 18299, NUITKA_BYTECODE_FLAG },
    { "weakref", NULL, 5661081, 19585, NUITKA_BYTECODE_FLAG },
    { "webbrowser", NULL, 5680666, 16385, NUITKA_BYTECODE_FLAG },
    { "wsgiref", NULL, 5697051, 749, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "wsgiref.handlers", NULL, 5697800, 16301, NUITKA_BYTECODE_FLAG },
    { "wsgiref.headers", NULL, 5714101, 7770, NUITKA_BYTECODE_FLAG },
    { "wsgiref.simple_server", NULL, 5721871, 5216, NUITKA_BYTECODE_FLAG },
    { "wsgiref.util", NULL, 5727087, 5191, NUITKA_BYTECODE_FLAG },
    { "wsgiref.validate", NULL, 5732278, 14687, NUITKA_BYTECODE_FLAG },
    { "xdrlib", NULL, 5746965, 8335, NUITKA_BYTECODE_FLAG },
    { "xml", NULL, 5755300, 717, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "xml.dom", NULL, 5756017, 5469, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "xml.dom.NodeFilter", NULL, 5761486, 984, NUITKA_BYTECODE_FLAG },
    { "xml.dom.domreg", NULL, 5762470, 2861, NUITKA_BYTECODE_FLAG },
    { "xml.dom.expatbuilder", NULL, 5765331, 27031, NUITKA_BYTECODE_FLAG },
    { "xml.dom.minicompat", NULL, 5792362, 2830, NUITKA_BYTECODE_FLAG },
    { "xml.dom.minidom", NULL, 5795192, 55630, NUITKA_BYTECODE_FLAG },
    { "xml.dom.pulldom", NULL, 5850822, 10503, NUITKA_BYTECODE_FLAG },
    { "xml.dom.xmlbuilder", NULL, 5861325, 12450, NUITKA_BYTECODE_FLAG },
    { "xml.etree", NULL, 5873775, 156, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "xml.etree.ElementInclude", NULL, 5873931, 1592, NUITKA_BYTECODE_FLAG },
    { "xml.etree.ElementPath", NULL, 5875523, 6360, NUITKA_BYTECODE_FLAG },
    { "xml.etree.ElementTree", NULL, 5881883, 44824, NUITKA_BYTECODE_FLAG },
    { "xml.etree.cElementTree", NULL, 5926707, 198, NUITKA_BYTECODE_FLAG },
    { "xml.parsers", NULL, 5926905, 330, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "xml.parsers.expat", NULL, 5927235, 359, NUITKA_BYTECODE_FLAG },
    { "xml.sax", NULL, 5927594, 3189, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "xml.sax._exceptions", NULL, 5930783, 5498, NUITKA_BYTECODE_FLAG },
    { "xml.sax.expatreader", NULL, 5936281, 12401, NUITKA_BYTECODE_FLAG },
    { "xml.sax.handler", NULL, 5948682, 12374, NUITKA_BYTECODE_FLAG },
    { "xml.sax.saxutils", NULL, 5961056, 12827, NUITKA_BYTECODE_FLAG },
    { "xml.sax.xmlreader", NULL, 5973883, 16935, NUITKA_BYTECODE_FLAG },
    { "xmlrpc", NULL, 5990818, 153, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { "xmlrpc.client", NULL, 5990971, 34559, NUITKA_BYTECODE_FLAG },
    { "xmlrpc.server", NULL, 6025530, 29401, NUITKA_BYTECODE_FLAG },
    { "zipapp", NULL, 6054931, 5814, NUITKA_BYTECODE_FLAG },
    { "zipfile", NULL, 6060745, 49898, NUITKA_BYTECODE_FLAG },
    { NULL, NULL, 0, 0, 0 }
};

void setupMetaPathBasedLoader( void )
{
    static bool init_done = false;

    if ( init_done == false )
    {
        registerMetaPathBasedUnfreezer( meta_path_loader_entries );
        init_done = true;
    }
}
