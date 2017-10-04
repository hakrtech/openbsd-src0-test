from lldbsuite.test import lldbinline
from lldbsuite.test import decorators

lldbinline.MakeInlineTest(
    __file__, globals(), [
        decorators.skipIfFreeBSD, decorators.skipIfLinux, decorators.skipIfWindows, decorators.skipIf(archs=["i386", "i686"])])
