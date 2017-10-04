"""
Test that recursive types are handled correctly.
"""

from __future__ import print_function


import lldb
import lldbsuite.test.lldbutil as lldbutil
import sys
from lldbsuite.test.lldbtest import *


class RecursiveTypesTestCase(TestBase):

    mydir = TestBase.compute_mydir(__file__)

    def setUp(self):
        # Call super's setUp().
        TestBase.setUp(self)
        # disable "There is a running process, kill it and restart?" prompt
        self.runCmd("settings set auto-confirm true")
        self.addTearDownHook(
            lambda: self.runCmd("settings clear auto-confirm"))
        # Find the line number to break for main.c.
        self.line = line_number('recursive_type_main.cpp',
                                '// Test at this line.')

        self.d1 = {
            'CXX_SOURCES': 'recursive_type_main.cpp recursive_type_1.cpp'}
        self.d2 = {
            'CXX_SOURCES': 'recursive_type_main.cpp recursive_type_2.cpp'}

    def test_recursive_type_1(self):
        """Test that recursive structs are displayed correctly."""
        self.build(dictionary=self.d1)
        self.setTearDownCleanup(dictionary=self.d1)
        self.print_struct()

    def test_recursive_type_2(self):
        """Test that recursive structs are displayed correctly."""
        self.build(dictionary=self.d2)
        self.setTearDownCleanup(dictionary=self.d2)
        self.print_struct()

    def print_struct(self):
        self.runCmd("file a.out", CURRENT_EXECUTABLE_SET)

        lldbutil.run_break_set_by_file_and_line(
            self,
            "recursive_type_main.cpp",
            self.line,
            num_expected_locations=-1,
            loc_exact=True)

        self.runCmd("run", RUN_SUCCEEDED)

        self.expect("print tpi", RUN_SUCCEEDED)
        self.expect("print *tpi", RUN_SUCCEEDED)
