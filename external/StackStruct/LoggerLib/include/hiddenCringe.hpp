#ifndef HIDDEN_CRINGE_LIB
#define HIDDEN_CRINGE_LIB

#define _fe_0(_stream, _call, x, ...)
#define _fe_1(_stream, _call, x, ...) _call(_stream, x)
#define _fe_2(_stream, _call, x, ...) _call(_stream, x) _fe_1(_stream, _call, __VA_ARGS__)
#define _fe_3(_stream, _call, x, ...) _call(_stream, x) _fe_2(_stream, _call, __VA_ARGS__)
#define _fe_3(_stream, _call, x, ...) _call(_stream, x) _fe_2(_stream, _call, __VA_ARGS__)
#define _fe_4(_stream, _call, x, ...) _call(_stream, x) _fe_3(_stream, _call, __VA_ARGS__)
#define _fe_5(_stream, _call, x, ...) _call(_stream, x) _fe_4(_stream, _call, __VA_ARGS__)

#define _GET_NTH_ARG(_1, _2, _3, _4, _5, _6, N, ...) N

#endif
