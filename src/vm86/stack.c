/*!The x86 Script Instruction Virtual Machine
 * 
 * vm86 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * vm86 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with vm86; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2014 - 2016, ruki All rights reserved.
 *
 * @author      ruki
 * @file        stack.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "machine_stack"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "machine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the machine stack stack type
typedef struct __vm86_stack_t
{
    // the data
    tb_uint32_t*    data;

    // the size
    tb_size_t       size;

    // the top
    tb_uint32_t**   top;

}vm86_stack_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
vm86_stack_ref_t vm86_stack_init(tb_size_t size, tb_uint32_t* esp)
{
    // check
    tb_assert_and_check_return_val(size && esp, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    vm86_stack_t*   stack = tb_null;
    do
    {
        // make stack
        stack = tb_malloc0_type(vm86_stack_t);
        tb_assert_and_check_break(stack);

        // make data
        stack->data = tb_nalloc0_type(size, tb_uint32_t);
        tb_assert_and_check_break(stack->data);

        // init stack
        stack->size  = size;

        // init top
        stack->top   = (tb_uint32_t**)esp;
        *stack->top  = stack->data + size;
    
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stack) vm86_stack_exit((vm86_stack_ref_t)stack);
        stack = tb_null;
    }

    // ok?
    return (vm86_stack_ref_t)stack;
}
tb_void_t vm86_stack_exit(vm86_stack_ref_t self)
{
    // check
    vm86_stack_t* stack = (vm86_stack_t*)self;
    tb_assert_and_check_return(stack);

    // exit data
    if (stack->data) tb_free(stack->data);
    stack->data = tb_null;

    // exit it
    tb_free(stack);
}
tb_void_t vm86_stack_clear(vm86_stack_ref_t self)
{
    // check
    vm86_stack_t* stack = (vm86_stack_t*)self;
    tb_assert_and_check_return(stack && stack->top && stack->data && stack->size);

    // clear data
    tb_memset(stack->data, 0, stack->size * sizeof(tb_uint32_t));

    // reset it
    *stack->top = stack->data + stack->size;
}
tb_void_t vm86_stack_top(vm86_stack_ref_t self, tb_uint32_t* pdata, tb_size_t index)
{
    // check
    vm86_stack_t* stack = (vm86_stack_t*)self;
    tb_assert_and_check_return(stack && pdata);

    // check
    tb_assert(stack->top && *stack->top);
    tb_assert(*stack->top + index < stack->data + stack->size);

    // save data
    *pdata = (*stack->top)[index];
}
tb_void_t vm86_stack_push(vm86_stack_ref_t self, tb_uint32_t data)
{
    // check
    vm86_stack_t* stack = (vm86_stack_t*)self;
    tb_assert_and_check_return(stack);

    // check
    tb_assert(stack->top && *stack->top);
    tb_assert(*stack->top > stack->data);

    // push it
    *--(*stack->top) = data;
}
tb_void_t vm86_stack_pop(vm86_stack_ref_t self, tb_uint32_t* pdata)
{
    // check
    vm86_stack_t* stack = (vm86_stack_t*)self;
    tb_assert_and_check_return(stack);

    // check
    tb_assert(stack->top && *stack->top);
    tb_assert(*stack->top < stack->data + stack->size);

    // save data
    if (pdata) *pdata = **stack->top;

    // pop it
    (*stack->top)++;
}
#ifdef __vm_debug__
tb_void_t vm86_stack_dump(vm86_stack_ref_t self)
{
    // check
    vm86_stack_t* stack = (vm86_stack_t*)self;
    tb_assert_and_check_return(stack);

    // trace
    tb_trace_i("");
    tb_trace_i("stack: ");

    // done
    tb_uint32_t* top = (*stack->top);
    tb_uint32_t* end = stack->data + stack->size;
    while (top < end)
    {
        // trace
        tb_trace_i("%p: %#x", top, *top);

        // next
        top++;
    }
}
#endif
