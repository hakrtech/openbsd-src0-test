﻿"""
Test 'watchpoint command'.
"""

from __future__ import print_function


import os
import time
import lldb
from lldbsuite.test.decorators import *
from lldbsuite.test.lldbtest import *
from lldbsuite.test import lldbutil


class WatchpointLLDBCommandTestCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    def setUp(self):
        # Call super's setUp().
        TestBase.setUp(self)
        # Our simple source filename.
        self.source = 'main.cpp'
        # Find the line number to break inside main().
        self.line = line_number(
            self.source, '// Set break point at this line.')
        # And the watchpoint variable declaration line number.
        self.decl = line_number(self.source,
                                '// Watchpoint variable declaration.')
        # Build dictionary to have unique executable names for each test
        # method.
        self.exe_name = 'a%d.out' % self.test_number
        self.d = {'CXX_SOURCES': self.source, 'EXE': self.exe_name}

    # Watchpoints not supported
    @expectedFailureAndroid(archs=['arm', 'aarch64'])
    @expectedFailureAll(
        oslist=["linux"],
        archs=["aarch64"],
        bugnumber="llvm.org/pr27710")
    @expectedFailureAll(
        oslist=["windows"],
        bugnumber="llvm.org/pr24446: WINDOWS XFAIL TRIAGE - Watchpoints not supported on Windows")
    def test_watchpoint_command(self):
        """Test 'watchpoint command'."""
        self.build(dictionary=self.d)
        self.setTearDownCleanup(dictionary=self.d)

        exe = os.path.join(os.getcwd(), self.exe_name)
        self.runCmd("file " + exe, CURRENT_EXECUTABLE_SET)

        # Add a breakpoint to set a watchpoint when stopped on the breakpoint.
        lldbutil.run_break_set_by_file_and_line(
            self, None, self.line, num_expected_locations=1)

        # Run the program.
        self.runCmd("run", RUN_SUCCEEDED)

        # We should be stopped again due to the breakpoint.
        # The stop reason of the thread should be breakpoint.
        self.expect("thread list", STOPPED_DUE_TO_BREAKPOINT,
                    substrs=['stopped',
                             'stop reason = breakpoint'])

        # Now let's set a write-type watchpoint for 'global'.
        self.expect(
            "watchpoint set variable -w write global",
            WATCHPOINT_CREATED,
            substrs=[
                'Watchpoint created',
                'size = 4',
                'type = w',
                '%s:%d' %
                (self.source,
                 self.decl)])

        self.runCmd('watchpoint command add 1 -o "expr -- cookie = 777"')

        # List the watchpoint command we just added.
        self.expect("watchpoint command list 1",
                    substrs=['expr -- cookie = 777'])

        # Use the '-v' option to do verbose listing of the watchpoint.
        # The hit count should be 0 initially.
        self.expect("watchpoint list -v",
                    substrs=['hit_count = 0'])

        self.runCmd("process continue")

        # We should be stopped again due to the watchpoint (write type).
        # The stop reason of the thread should be watchpoint.
        self.expect("thread backtrace", STOPPED_DUE_TO_WATCHPOINT,
                    substrs=['stop reason = watchpoint'])

        # Check that the watchpoint snapshoting mechanism is working.
        self.expect("watchpoint list -v",
                    substrs=['old value:', ' = 0',
                             'new value:', ' = 1'])

        # The watchpoint command "forced" our global variable 'cookie' to
        # become 777.
        self.expect("frame variable --show-globals cookie",
                    substrs=['(int32_t)', 'cookie = 777'])

    # Watchpoints not supported
    @expectedFailureAndroid(archs=['arm', 'aarch64'])
    @expectedFailureAll(
        oslist=["linux"],
        archs=["aarch64"],
        bugnumber="llvm.org/pr27710")
    @expectedFailureAll(
        oslist=["windows"],
        bugnumber="llvm.org/pr24446: WINDOWS XFAIL TRIAGE - Watchpoints not supported on Windows")
    def test_watchpoint_command_can_disable_a_watchpoint(self):
        """Test that 'watchpoint command' action can disable a watchpoint after it is triggered."""
        self.build(dictionary=self.d)
        self.setTearDownCleanup(dictionary=self.d)

        exe = os.path.join(os.getcwd(), self.exe_name)
        self.runCmd("file " + exe, CURRENT_EXECUTABLE_SET)

        # Add a breakpoint to set a watchpoint when stopped on the breakpoint.
        lldbutil.run_break_set_by_file_and_line(
            self, None, self.line, num_expected_locations=1)

        # Run the program.
        self.runCmd("run", RUN_SUCCEEDED)

        # We should be stopped again due to the breakpoint.
        # The stop reason of the thread should be breakpoint.
        self.expect("thread list", STOPPED_DUE_TO_BREAKPOINT,
                    substrs=['stopped',
                             'stop reason = breakpoint'])

        # Now let's set a write-type watchpoint for 'global'.
        self.expect(
            "watchpoint set variable -w write global",
            WATCHPOINT_CREATED,
            substrs=[
                'Watchpoint created',
                'size = 4',
                'type = w',
                '%s:%d' %
                (self.source,
                 self.decl)])

        self.runCmd('watchpoint command add 1 -o "watchpoint disable 1"')

        # List the watchpoint command we just added.
        self.expect("watchpoint command list 1",
                    substrs=['watchpoint disable 1'])

        # Use the '-v' option to do verbose listing of the watchpoint.
        # The hit count should be 0 initially.
        self.expect("watchpoint list -v",
                    substrs=['hit_count = 0'])

        self.runCmd("process continue")

        # We should be stopped again due to the watchpoint (write type).
        # The stop reason of the thread should be watchpoint.
        self.expect("thread backtrace", STOPPED_DUE_TO_WATCHPOINT,
                    substrs=['stop reason = watchpoint'])

        # Check that the watchpoint has been disabled.
        self.expect("watchpoint list -v",
                    substrs=['disabled'])

        self.runCmd("process continue")

        # There should be no more watchpoint hit and the process status should
        # be 'exited'.
        self.expect("process status",
                    substrs=['exited'])
