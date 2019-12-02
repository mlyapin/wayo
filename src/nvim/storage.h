/* Copyright (c) 2019 Maxim Lyapin 
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy 
 *  of this software and associated documentation files (the "Software"), to deal 
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
 *  SOFTWARE.
 */

#pragma once

#include <stddef.h>

#include "api.h"

/**
 * @brief A pointer to a function to call when neovim will send a response on
 * the request.
 */
typedef void (*request_callback)(void);
/**
 * @brief A structure that represents a request to the neovim side.
 */
struct request {
	union {
		nvim_rpc_msgid msgid;
		int64_t id;
	};
	uv_write_t *uvreq;
	request_callback callb;
};

// TODO(Maxim Lyapin): Right now, the storage is implemented as a very simple
// hash table implemented by hands. This will work in most cases, but there are
// edge cases (neovim may suspend request processing). So it would be nice to
// replace it with some kind of binary-search tree or "big" hash table.

/**
 * @brief A storage for all request that "wait" for a responses. Storage itself
 * is thread-safe, but individual requests don't.
 */
struct request_storage {
	mtx_t lock;
	struct request *data;
	int length;
	int capacity;
};

/**
 * @brief Return codes enumeration that represents result of operations on the
 * request storage.
 */
enum storage_rc {
	STORAGE_RC_OK = 0,
	STORAGE_RC_NOSPACE,
	STORAGE_RC_DUPLICATE,
	STORAGE_RC_NOTFOUND,
};

void request_storage_init(struct request_storage *, int capacity);
void request_storage_destroy(struct request_storage *);

/**
 * @brief Creates new request (allocates memory for it and so on) and returns a
 * pointer to created value.
 *
 * @param requess storage object on which the operation should be performed.
 * @param nvim_rpc_msgid id of a request in the question.
 * @param request_callback a function pointer which will be called on neovim's
 * response.
 * @param result a pointer on a pointer on a created structure. Can be NULL.
 *
 * @return return code that represents a status of performed operation.
 */
enum storage_rc request_storage_create_req(struct request_storage *,
					   nvim_rpc_msgid, request_callback,
					   struct request **result);

/**
 * @brief Searches for a request with a given msgid.
 *
 * @param request storage object on which the operation should be performed.
 * @param nvim_rpc_msgid id of the request.
 * @param result a pointer on a pointer on a structure that was found. Will be
 * NULL if such a request is not exists.
 *
 * @return return code that represents a status of performed operation.
 */
enum storage_rc request_storage_find(struct request_storage *, nvim_rpc_msgid,
				     struct request **result);

/**
 * @brief Frees all the resources occupied by a request with given id.
 *
 * @param request storage object on which the operation should be performed.
 * @param nvim_rpc_msgid id of the request.
 *
 * @return return code.
 */
enum storage_rc request_storage_free_req(struct request_storage *,
					 nvim_rpc_msgid);
