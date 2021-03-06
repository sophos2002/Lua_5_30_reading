/*
** $Id: lmem.h,v 1.43 2014/12/19 17:26:14 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/

#ifndef lmem_h
#define lmem_h


#include <stddef.h>

#include "llimits.h"
#include "lua.h"


/*
** This macro reallocs a vector 'b' from 'on' to 'n' elements, where
** each element has size 'e'. In case of arithmetic overflow of the
** product 'n'*'e', it raises an error (calling 'luaM_toobig'). Because
** 'e' is always constant, it avoids the runtime division MAX_SIZET/(e).
**
** (The macro is somewhat complex to avoid warnings:  The 'sizeof'
** comparison avoids a runtime comparison when overflow cannot occur.
** The compiler should be able to optimize the real test by itself, but
** when it does it, it may give a warning about "comparison is always
** false due to limited range of data type"; the +1 tricks the compiler,
** avoiding this warning but also this optimization.)
*/
/**
 * TODO +1 为何可以消除编译器警告?
 */
#define luaM_reallocv(L,b,on,n,e) \
  (((sizeof(n) >= sizeof(size_t) && cast(size_t, (n)) + 1 > MAX_SIZET/(e)) \
      ? luaM_toobig(L) : cast_void(0)) , \
   luaM_realloc_(L, (b), (on)*(e), (n)*(e)))

/*
** Arrays of chars do not need any test
*/
#define luaM_reallocvchar(L,b,on,n)  \
    cast(char *, luaM_realloc_(L, (b), (on)*sizeof(char), (n)*sizeof(char)))

/* 释放 b 指向的内存, s 为内存块大小 */
#define luaM_freemem(L, b, s)	luaM_realloc_(L, (b), (s), 0)
/* 
 * 释放对象b所占内存
 * b: 地址
 */
#define luaM_free(L, b)		luaM_realloc_(L, (b), sizeof(*(b)), 0)
/* 
 * b: 地址块地址; 
 * n: 元素个数 
 */ 
#define luaM_freearray(L, b, n)   luaM_realloc_(L, (b), (n)*sizeof(*(b)), 0)

#define luaM_malloc(L,s)	luaM_realloc_(L, NULL, 0, (s))
#define luaM_new(L,t)		cast(t *, luaM_malloc(L, sizeof(t)))
/* 新建一个容器，包含 n 个 t 类型的元素 */
#define luaM_newvector(L,n,t) \
		cast(t *, luaM_reallocv(L, NULL, 0, n, sizeof(t)))

/* 新建一个大小为s的对象, 实质就是分配一块大小为s的内存 */
#define luaM_newobject(L,tag,s)	luaM_realloc_(L, NULL, tag, (s))

/**
 * nelems: 容器中元素个数
 * size: 容器大小
 */
#define luaM_growvector(L,v,nelems,size,t,limit,e) \
          if ((nelems)+1 > (size)) \
            ((v)=cast(t *, luaM_growaux_(L,v,&(size),sizeof(t),limit,e)))

/**
 * 重新分配 vector 的大小, oldn 为原来的大小, n 为新的大小,
 * v 是指向vector的指针, t 是vector保存的数据类型
 */
#define luaM_reallocvector(L, v,oldn,n,t) \
   ((v)=cast(t *, luaM_reallocv(L, v, oldn, n, sizeof(t))))

/* 内存分配错误， 用户需要分配的block太大, 超出了Lua的限制 */
LUAI_FUNC l_noret luaM_toobig (lua_State *L);

/* not to be called directly */
LUAI_FUNC void *luaM_realloc_ (lua_State *L, void *block, size_t oldsize,
                                                          size_t size);
LUAI_FUNC void *luaM_growaux_ (lua_State *L, void *block, int *size,
                               size_t size_elem, int limit,
                               const char *what);

#endif

