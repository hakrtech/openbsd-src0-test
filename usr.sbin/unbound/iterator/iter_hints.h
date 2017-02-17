/*
 * iterator/iter_hints.h - iterative resolver module stub and root hints.
 *
 * Copyright (c) 2007, NLnet Labs. All rights reserved.
 *
 * This software is open source.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * Neither the name of the NLNET LABS nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * This file contains functions to assist the iterator module.
 * Keep track of stub and root hints, and read those from config.
 */

#ifndef ITERATOR_ITER_HINTS_H
#define ITERATOR_ITER_HINTS_H
#include "util/storage/dnstree.h"
struct iter_env;
struct config_file;
struct delegpt;

/**
 * Iterator hints structure
 */
struct iter_hints {
	/** 
	 * Hints are stored in this tree. Sort order is specially chosen.
	 * first sorted on qclass. Then on dname in nsec-like order, so that
	 * a lookup on class, name will return an exact match or the closest
	 * match which gives the ancestor needed.
	 * contents of type iter_hints_stub. The class IN root is in here.
	 * uses name_tree_node from dnstree.h.
	 */
	rbtree_type tree;
};

/**
 * Iterator hints for a particular stub.
 */
struct iter_hints_stub {
	/** tree sorted by name, class */
	struct name_tree_node node;
	/** delegation point with hint information for this stub. malloced. */
	struct delegpt* dp;
	/** does the stub need to forego priming (like on other ports) */
	uint8_t noprime;
};

/**
 * Create hints 
 * @return new hints or NULL on error.
 */
struct iter_hints* hints_create(void);

/**
 * Delete hints.
 * @param hints: to delete.
 */
void hints_delete(struct iter_hints* hints);

/**
 * Process hints config. Sets default values for root hints if no config.
 * @param hints: where to store.
 * @param cfg: config options.
 * @return 0 on error.
 */
int hints_apply_cfg(struct iter_hints* hints, struct config_file* cfg);

/**
 * Find root hints for the given class.
 * @param hints: hint storage.
 * @param qclass: class for which root hints are requested. host order.
 * @return: NULL if no hints, or a ptr to stored hints.
 */
struct delegpt* hints_lookup_root(struct iter_hints* hints, uint16_t qclass);

/**
 * Find next root hints (to cycle through all root hints).
 * @param hints: hint storage
 * @param qclass: class for which root hints are sought.
 * 	0 means give the first available root hints class.
 * 	x means, give class x or a higher class if any.
 * 	returns the found class in this variable.
 * @return true if a root hint class is found.
 * 	false if not root hint class is found (qclass may have been changed).
 */
int hints_next_root(struct iter_hints* hints, uint16_t* qclass);

/**
 * Given a qname/qclass combination, and the delegation point from the cache
 * for this qname/qclass, determine if this combination indicates that a
 * stub hint exists and must be primed.
 *
 * @param hints: hint storage.
 * @param qname: The qname that generated the delegation point.
 * @param qclass: The qclass that generated the delegation point.
 * @param dp: The cache generated delegation point.
 * @return: A priming delegation point if there is a stub hint that must
 *         be primed, otherwise null.
 */
struct iter_hints_stub* hints_lookup_stub(struct iter_hints* hints, 
	uint8_t* qname, uint16_t qclass, struct delegpt* dp);

/**
 * Get memory in use by hints
 * @param hints: hint storage.
 * @return bytes in use
 */
size_t hints_get_mem(struct iter_hints* hints);

/**
 * Add stub to hints structure. For external use since it recalcs 
 * the tree parents.
 * @param hints: the hints data structure
 * @param c: class of zone
 * @param dp: delegation point with name and target nameservers for new
 *	hints stub. malloced.
 * @param noprime: set noprime option to true or false on new hint stub.
 * @return false on failure (out of memory);
 */
int hints_add_stub(struct iter_hints* hints, uint16_t c, struct delegpt* dp,
	int noprime);

/**
 * Remove stub from hints structure. For external use since it 
 * recalcs the tree parents.
 * @param hints: the hints data structure
 * @param c: class of stub zone
 * @param nm: name of stub zone (in uncompressed wireformat).
 */
void hints_delete_stub(struct iter_hints* hints, uint16_t c, uint8_t* nm);

#endif /* ITERATOR_ITER_HINTS_H */
