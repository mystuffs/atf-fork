//
// Automated Testing Framework (atf)
//
// Copyright (c) 2007 The NetBSD Foundation, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this
//    software must display the following acknowledgement:
//        This product includes software developed by the NetBSD
//        Foundation, Inc. and its contributors.
// 4. Neither the name of The NetBSD Foundation nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
// CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
}

#include <fstream>
#include <stdexcept>

#include "atf/env.hpp"
#include "atf/fs.hpp"
#include "atf/macros.hpp"

// ------------------------------------------------------------------------
// Auxiliary functions.
// ------------------------------------------------------------------------

static
void
safe_mkdir(const char* path)
{
    if (::mkdir(path, 0755) == -1)
        ATF_FAIL(std::string("mkdir(2) of ") + path + " failed");
}

static
void
touch(const char* path)
{
    std::ofstream os(path);
    if (!os)
        ATF_FAIL(std::string("Could not create file ") + path);
    os.close();
}

// ------------------------------------------------------------------------
// Helper tests for "t_config".
// ------------------------------------------------------------------------

ATF_TEST_CASE(config_unset);
ATF_TEST_CASE_HEAD(config_unset)
{
    set("descr", "Helper test case for the t_config test program");
}
ATF_TEST_CASE_BODY(config_unset)
{
    ATF_CHECK(!config().has("test"));
}

ATF_TEST_CASE(config_empty);
ATF_TEST_CASE_HEAD(config_empty)
{
    set("descr", "Helper test case for the t_config test program");
}
ATF_TEST_CASE_BODY(config_empty)
{
    ATF_CHECK_EQUAL(config().get("test"), "");
}

ATF_TEST_CASE(config_value);
ATF_TEST_CASE_HEAD(config_value)
{
    set("descr", "Helper test case for the t_config test program");
}
ATF_TEST_CASE_BODY(config_value)
{
    ATF_CHECK_EQUAL(config().get("test"), "foo");
}

ATF_TEST_CASE(config_multi_value);
ATF_TEST_CASE_HEAD(config_multi_value)
{
    set("descr", "Helper test case for the t_config test program");
}
ATF_TEST_CASE_BODY(config_multi_value)
{
    ATF_CHECK_EQUAL(config().get("test"), "foo bar");
}

// ------------------------------------------------------------------------
// Helper tests for "t_env".
// ------------------------------------------------------------------------

ATF_TEST_CASE(env_home);
ATF_TEST_CASE_HEAD(env_home)
{
    set("descr", "Helper test case for the t_env test program");
}
ATF_TEST_CASE_BODY(env_home)
{
    ATF_CHECK(atf::env::has("HOME"));
    atf::fs::file_info fi1(atf::fs::path(atf::env::get("HOME")));
    atf::fs::file_info fi2(atf::fs::get_current_dir());
    ATF_CHECK_EQUAL(fi1.get_device(), fi2.get_device());
    ATF_CHECK_EQUAL(fi1.get_inode(), fi2.get_inode());
}

ATF_TEST_CASE(env_undef);
ATF_TEST_CASE_HEAD(env_undef)
{
    set("descr", "Helper test case for the t_env test program");
}
ATF_TEST_CASE_BODY(env_undef)
{
    ATF_CHECK(!atf::env::has("LC_COLLATE"));
    ATF_CHECK(!atf::env::has("TZ"));
}

// ------------------------------------------------------------------------
// Helper tests for "t_fork".
// ------------------------------------------------------------------------

ATF_TEST_CASE(fork_mangle_fds);
ATF_TEST_CASE_HEAD(fork_mangle_fds)
{
    set("descr", "Helper test case for the t_fork test program");
    set("isolated", config().get("isolated", "yes"));
}
ATF_TEST_CASE_BODY(fork_mangle_fds)
{
    int resfd = std::atoi(config().get("resfd").c_str());

    if (::close(STDIN_FILENO) == -1)
        ATF_FAIL("Failed to close stdin");
    if (::close(STDOUT_FILENO) == -1)
        ATF_FAIL("Failed to close stdout");
    if (::close(STDERR_FILENO) == -1)
        ATF_FAIL("Failed to close stderr");
    if (::close(resfd) == -1)
        ATF_FAIL("Failed to close results descriptor");

#if defined(F_CLOSEM)
    if (::fcntl(0, F_CLOSEM) == -1)
        ATF_FAIL("Failed to close everything");
#endif
}

// ------------------------------------------------------------------------
// Helper tests for "t_isolated".
// ------------------------------------------------------------------------

ATF_TEST_CASE(isolated_path);
ATF_TEST_CASE_HEAD(isolated_path)
{
    set("descr", "Helper test case for the t_isolated test program");
    set("isolated", config().get("isolated", "yes"));
}
ATF_TEST_CASE_BODY(isolated_path)
{
    const std::string& p = config().get("pathfile");

    std::ofstream os(p.c_str());
    if (!os)
        ATF_FAIL("Could not open " + p + " for writing");

    os << atf::fs::get_current_dir().str() << std::endl;

    os.close();
}

ATF_TEST_CASE(isolated_cleanup);
ATF_TEST_CASE_HEAD(isolated_cleanup)
{
    set("descr", "Helper test case for the t_isolated test program");
    set("isolated", "yes");
}
ATF_TEST_CASE_BODY(isolated_cleanup)
{
    const std::string& p = config().get("pathfile");

    std::ofstream os(p.c_str());
    if (!os)
        ATF_FAIL("Could not open " + p + " for writing");

    os << atf::fs::get_current_dir().str() << std::endl;

    os.close();

    safe_mkdir("1");
    safe_mkdir("1/1");
    safe_mkdir("1/2");
    safe_mkdir("1/3");
    safe_mkdir("1/3/1");
    safe_mkdir("1/3/2");
    safe_mkdir("2");
    touch("2/1");
    touch("2/2");
    safe_mkdir("2/3");
    touch("2/3/1");
}

// ------------------------------------------------------------------------
// Helper tests for "t_srcdir".
// ------------------------------------------------------------------------

ATF_TEST_CASE(srcdir_exists);
ATF_TEST_CASE_HEAD(srcdir_exists)
{
    set("descr", "Helper test case for the t_srcdir test program");
}
ATF_TEST_CASE_BODY(srcdir_exists)
{
    if (!atf::fs::exists(atf::fs::path(get_srcdir()) / "datafile"))
        ATF_FAIL("Cannot find datafile");
}

// ------------------------------------------------------------------------
// Helper tests for "t_require_progs".
// ------------------------------------------------------------------------

ATF_TEST_CASE(require_progs_body);
ATF_TEST_CASE_HEAD(require_progs_body)
{
    set("descr", "Helper test case for the t_require_progs test program");
}
ATF_TEST_CASE_BODY(require_progs_body)
{
    require_prog(config().get("progs"));
}

ATF_TEST_CASE(require_progs_head);
ATF_TEST_CASE_HEAD(require_progs_head)
{
    set("descr", "Helper test case for the t_require_head test program");
    set("require.progs", config().get("progs", "not-set"));
}
ATF_TEST_CASE_BODY(require_progs_head)
{
}

// ------------------------------------------------------------------------
// Helper tests for "t_require_user".
// ------------------------------------------------------------------------

ATF_TEST_CASE(require_user_root);
ATF_TEST_CASE_HEAD(require_user_root)
{
    set("descr", "Helper test case for the t_require_user test program");
    set("isolated", "no");
    set("require.user", "root");
}
ATF_TEST_CASE_BODY(require_user_root)
{
}

ATF_TEST_CASE(require_user_root2);
ATF_TEST_CASE_HEAD(require_user_root2)
{
    set("descr", "Helper test case for the t_require_user test program");
    set("isolated", "no");
    set("require.user", "root");
}
ATF_TEST_CASE_BODY(require_user_root2)
{
}

ATF_TEST_CASE(require_user_unprivileged);
ATF_TEST_CASE_HEAD(require_user_unprivileged)
{
    set("descr", "Helper test case for the t_require_user test program");
    set("isolated", "no");
    set("require.user", "unprivileged");
}
ATF_TEST_CASE_BODY(require_user_unprivileged)
{
}

ATF_TEST_CASE(require_user_unprivileged2);
ATF_TEST_CASE_HEAD(require_user_unprivileged2)
{
    set("descr", "Helper test case for the t_require_user test program");
    set("isolated", "no");
    set("require.user", "unprivileged");
}
ATF_TEST_CASE_BODY(require_user_unprivileged2)
{
}

// ------------------------------------------------------------------------
// Main.
// ------------------------------------------------------------------------

ATF_INIT_TEST_CASES(tcs)
{
    // Add helper tests for t_config.
    tcs.push_back(&config_unset);
    tcs.push_back(&config_empty);
    tcs.push_back(&config_value);
    tcs.push_back(&config_multi_value);

    // Add helper tests for t_env.
    tcs.push_back(&env_home);
    tcs.push_back(&env_undef);

    // Add helper tests for t_fork.
    tcs.push_back(&fork_mangle_fds);

    // Add helper tests for t_isolated.
    tcs.push_back(&isolated_path);
    tcs.push_back(&isolated_cleanup);

    // Add helper tests for t_srcdir.
    tcs.push_back(&srcdir_exists);

    // Add helper tests for t_require_progs.
    tcs.push_back(&require_progs_body);
    tcs.push_back(&require_progs_head);

    // Add helper tests for t_require_user.
    tcs.push_back(&require_user_root);
    tcs.push_back(&require_user_root2);
    tcs.push_back(&require_user_unprivileged);
    tcs.push_back(&require_user_unprivileged2);
}