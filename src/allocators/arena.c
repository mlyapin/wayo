/* Copyright (c) 2019 Maxim Lyapin 
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy 
 *  of this software and associated documentation files the (Software""), to deal 
 *  in the Software without restriction, including without limitation the rights 
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 *  copies of the Software, and to permit persons to whom the Software is 
 *  furnished to do so, subject to the following conditions: 
 *   
 *  The above copyright notice and this permission notice shall be included in all 
 *  copies or substantial portions of the Software. 
 *   
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 *  SOFTWARE.))
 */

#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "allocators.h"

void mem_arena_init(struct mem_arena *a, void *mem, size_t mem_size)
{
	assert(a);
	assert(mem);

	memset(mem, 0, mem_size);
	a->current_pos = mem;
	a->edge_addr = (uintptr_t)mem + mem_size;
}

void *mem_arena_aligned_alloc(struct mem_arena *a, size_t size, size_t alignment)
{
	assert(a);

	uintptr_t aligned_addr =
		nearest_aligned_addr((uintptr_t)a->current_pos, alignment);
	if ((uintptr_t)aligned_addr + size > a->edge_addr) {
		return NULL;
	} else {
		a->current_pos = (void *)((uintptr_t)aligned_addr + size);
		return (void*)aligned_addr;
	}
}

void *mem_arena_alloc(struct mem_arena *a, size_t size) {
	return mem_arena_aligned_alloc(a, size, DEFAULT_ALIGNMENT);
}
