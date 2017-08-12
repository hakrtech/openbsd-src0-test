/*
 * util/module.c - module interface
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
 * Implementation of module.h.
 */

#include "config.h"
#include "util/module.h"
#include "sldns/wire2str.h"

const char* 
strextstate(enum module_ext_state s)
{
	switch(s) {
	case module_state_initial: return "module_state_initial";
	case module_wait_reply: return "module_wait_reply";
	case module_wait_module: return "module_wait_module";
	case module_restart_next: return "module_restart_next";
	case module_wait_subquery: return "module_wait_subquery";
	case module_error: return "module_error";
	case module_finished: return "module_finished";
	}
	return "bad_extstate_value";
}

const char* 
strmodulevent(enum module_ev e)
{
	switch(e) {
	case module_event_new: return "module_event_new";
	case module_event_pass: return "module_event_pass";
	case module_event_reply: return "module_event_reply";
	case module_event_noreply: return "module_event_noreply";
	case module_event_capsfail: return "module_event_capsfail";
	case module_event_moddone: return "module_event_moddone";
	case module_event_error: return "module_event_error";
	}
	return "bad_event_value";
}

int
edns_known_options_init(struct module_env* env)
{
	env->edns_known_options_num = 0;
	env->edns_known_options = (struct edns_known_option*)calloc(
		MAX_KNOWN_EDNS_OPTS, sizeof(struct edns_known_option));
	if(!env->edns_known_options) return 0;
	return 1;
}

void
edns_known_options_delete(struct module_env* env)
{
	free(env->edns_known_options);
	env->edns_known_options = NULL;
	env->edns_known_options_num = 0;
}

int
edns_register_option(uint16_t opt_code, int bypass_cache_stage,
	int no_aggregation, struct module_env* env)
{
	size_t i;
	if(env->worker) {
		log_err("invalid edns registration: "
			"trying to register option after module init phase");
		return 0;
	}

	/**
	 * Checking if we are full first is faster but it does not provide
	 * the option to change the flags when the array is full.
	 * It only impacts unbound initialization, leave it for now.
	 */
	/* Check if the option is already registered. */
	for(i=0; i<env->edns_known_options_num; i++)
		if(env->edns_known_options[i].opt_code == opt_code)
			break;
	/* If it is not yet registered check if we have space to add a new one. */
	if(i == env->edns_known_options_num) {
		if(env->edns_known_options_num >= MAX_KNOWN_EDNS_OPTS) {
			log_err("invalid edns registration: maximum options reached");
			return 0;
		}
		env->edns_known_options_num++;
	}
	env->edns_known_options[i].opt_code = opt_code;
	env->edns_known_options[i].bypass_cache_stage = bypass_cache_stage;
	env->edns_known_options[i].no_aggregation = no_aggregation;
	return 1;
}

int
inplace_cb_register(void* cb, enum inplace_cb_list_type type, void* cbarg,
	struct module_env* env, int id)
{
	struct inplace_cb* callback;
	struct inplace_cb** prevp;
	if(env->worker) {
		log_err("invalid edns callback registration: "
			"trying to register callback after module init phase");
		return 0;
	}

	callback = (struct inplace_cb*)calloc(1, sizeof(*callback));
	if(callback == NULL) {
		log_err("out of memory during edns callback registration.");
		return 0;
	}
	callback->id = id;
	callback->next = NULL;
	callback->cb = cb;
	callback->cb_arg = cbarg;
	
	prevp = (struct inplace_cb**) &env->inplace_cb_lists[type];
	/* append at end of list */
	while(*prevp != NULL)
		prevp = &((*prevp)->next);
	*prevp = callback;
	return 1;
}

void
inplace_cb_delete(struct module_env* env, enum inplace_cb_list_type type,
	int id)
{
	struct inplace_cb* temp = env->inplace_cb_lists[type];
	struct inplace_cb* prev = NULL;

	while(temp) {
		if(temp->id == id) {
			if(!prev) {
				env->inplace_cb_lists[type] = temp->next;
				free(temp);
				temp = env->inplace_cb_lists[type];
			}
			else {
				prev->next = temp->next;
				free(temp);
				temp = prev->next;
			}
		}
		else {
			prev = temp;
			temp = temp->next;
		}
	}
}

struct edns_known_option*
edns_option_is_known(uint16_t opt_code, struct module_env* env)
{
	size_t i;
	for(i=0; i<env->edns_known_options_num; i++)
		if(env->edns_known_options[i].opt_code == opt_code)
			return env->edns_known_options + i;
	return NULL;
}

int
edns_bypass_cache_stage(struct edns_option* list, struct module_env* env)
{
	size_t i;
	for(; list; list=list->next)
		for(i=0; i<env->edns_known_options_num; i++)
			if(env->edns_known_options[i].opt_code == list->opt_code &&
				env->edns_known_options[i].bypass_cache_stage == 1)
					return 1;
	return 0;
}

int
unique_mesh_state(struct edns_option* list, struct module_env* env)
{
	size_t i;
	if(env->unique_mesh)
		return 1;
	for(; list; list=list->next)
		for(i=0; i<env->edns_known_options_num; i++)
			if(env->edns_known_options[i].opt_code == list->opt_code &&
				env->edns_known_options[i].no_aggregation == 1)
					return 1;
	return 0;
}

void
log_edns_known_options(enum verbosity_value level, struct module_env* env)
{
	size_t i;
	char str[32], *s;
	size_t slen;
	if(env->edns_known_options_num > 0 && verbosity >= level) {
		verbose(level, "EDNS known options:");
		verbose(level, "  Code:    Bypass_cache_stage: Aggregate_mesh:");
		for(i=0; i<env->edns_known_options_num; i++) {
			s = str;
			slen = sizeof(str);
			(void)sldns_wire2str_edns_option_code_print(&s, &slen,
				env->edns_known_options[i].opt_code);
			verbose(level, "  %-8.8s %-19s %-15s", str,
				env->edns_known_options[i].bypass_cache_stage?"YES":"NO",
				env->edns_known_options[i].no_aggregation?"NO":"YES");
		}
	}
}
