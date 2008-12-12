/*
 * Automated Testing Framework (atf)
 *
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>

#include <atf-c.h>

#include "atf-c/error.h"
#include "atf-c/map.h"

#define CE(stm) ATF_REQUIRE(!atf_is_error(stm))

/* ---------------------------------------------------------------------
 * Tests for the "atf_map" type.
 * --------------------------------------------------------------------- */

/*
 * Constructors and destructors.
 */

ATF_TC(map_init);
ATF_TC_HEAD(map_init, tc)
{
    atf_tc_set_md_var(tc, "descr", "Checks the atf_map_init function");
}
ATF_TC_BODY(map_init, tc)
{
    atf_map_t map;

    CE(atf_map_init(&map));
    ATF_REQUIRE_EQ(atf_map_size(&map), 0);
    atf_map_fini(&map);
}

/*
 * Getters.
 */

ATF_TC(find_c);
ATF_TC_HEAD(find_c, tc)
{
    atf_tc_set_md_var(tc, "descr", "Checks the atf_map_find_c function");
}
ATF_TC_BODY(find_c, tc)
{
    atf_map_t map;
    char val1[] = "V1";
    char val2[] = "V2";
    atf_map_citer_t iter;

    CE(atf_map_init(&map));
    CE(atf_map_insert(&map, "K1", val1, false));
    CE(atf_map_insert(&map, "K2", val2, false));

    iter = atf_map_find_c(&map, "K0");
    ATF_REQUIRE(atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));

    iter = atf_map_find_c(&map, "K1");
    ATF_REQUIRE(!atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));
    ATF_REQUIRE(strcmp(atf_map_citer_data(iter), "V1") == 0);

    iter = atf_map_find_c(&map, "K2");
    ATF_REQUIRE(!atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));
    ATF_REQUIRE(strcmp(atf_map_citer_data(iter), "V2") == 0);

    atf_map_fini(&map);
}

/*
 * Modifiers.
 */

ATF_TC(map_insert);
ATF_TC_HEAD(map_insert, tc)
{
    atf_tc_set_md_var(tc, "descr", "Checks the atf_map_insert function");
}
ATF_TC_BODY(map_insert, tc)
{
    atf_map_t map;
    char buf[] = "1st test string";
    char buf2[] = "2nd test string";
    const char *ptr;
    atf_map_citer_t iter;

    CE(atf_map_init(&map));

    printf("Inserting some values\n");
    ATF_REQUIRE_EQ(atf_map_size(&map), 0);
    CE(atf_map_insert(&map, "K1", buf, false));
    ATF_REQUIRE_EQ(atf_map_size(&map), 1);
    CE(atf_map_insert(&map, "K2", buf, false));
    ATF_REQUIRE_EQ(atf_map_size(&map), 2);
    CE(atf_map_insert(&map, "K3", buf, false));
    ATF_REQUIRE_EQ(atf_map_size(&map), 3);

    printf("Replacing a value\n");
    iter = atf_map_find_c(&map, "K3");
    ATF_REQUIRE(!atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));
    ptr = atf_map_citer_data(iter);
    ATF_REQUIRE_EQ(ptr, buf);
    CE(atf_map_insert(&map, "K3", buf2, false));
    ATF_REQUIRE_EQ(atf_map_size(&map), 3);
    iter = atf_map_find_c(&map, "K3");
    ATF_REQUIRE(!atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));
    ptr = atf_map_citer_data(iter);
    ATF_REQUIRE_EQ(ptr, buf2);

    atf_map_fini(&map);
}

/*
 * Other.
 */

ATF_TC(stable_keys);
ATF_TC_HEAD(stable_keys, tc)
{
    atf_tc_set_md_var(tc, "descr", "Checks that the keys do not change "
                      "even if their original values do");
}
ATF_TC_BODY(stable_keys, tc)
{
    atf_map_t map;
    atf_map_citer_t iter;
    char key[] = "K1";

    CE(atf_map_init(&map));

    CE(atf_map_insert(&map, key, strdup("test-value"), true));
    iter = atf_map_find_c(&map, "K1");
    ATF_REQUIRE(!atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));
    iter = atf_map_find_c(&map, "K2");
    ATF_REQUIRE(atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));

    strcpy(key, "K2");
    iter = atf_map_find_c(&map, "K1");
    ATF_REQUIRE(!atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));
    iter = atf_map_find_c(&map, "K2");
    ATF_REQUIRE(atf_equal_map_citer_map_citer(iter, atf_map_end_c(&map)));

    atf_map_fini(&map);
}

/* ---------------------------------------------------------------------
 * Main.
 * --------------------------------------------------------------------- */

ATF_TP_ADD_TCS(tp)
{
    /* Constructors and destructors. */
    ATF_TP_ADD_TC(tp, map_init);

    /* Getters. */
    ATF_TP_ADD_TC(tp, find_c);

    /* Modifiers. */
    ATF_TP_ADD_TC(tp, map_insert);

    /* Other. */
    ATF_TP_ADD_TC(tp, stable_keys);

    return atf_no_error();
}
