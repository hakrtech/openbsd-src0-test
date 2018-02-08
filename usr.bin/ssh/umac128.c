/* $OpenBSD: umac128.c,v 1.2 2018/02/08 04:12:32 dtucker Exp $ */

#define UMAC_OUTPUT_LEN	16
#define umac_new	umac128_new
#define umac_update	umac128_update
#define umac_final	umac128_final
#define umac_delete	umac128_delete
#define umac_ctx	umac128_ctx

#include "umac.c"
