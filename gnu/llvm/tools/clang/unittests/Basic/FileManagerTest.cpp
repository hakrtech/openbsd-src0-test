//===- unittests/Basic/FileMangerTest.cpp ------------ FileManger tests ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/Basic/FileManager.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/FileSystemStatCache.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/Support/Path.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace clang;

namespace {

// Used to create a fake file system for running the tests with such
// that the tests are not affected by the structure/contents of the
// file system on the machine running the tests.
class FakeStatCache : public FileSystemStatCache {
private:
  // Maps a file/directory path to its desired stat result.  Anything
  // not in this map is considered to not exist in the file system.
  llvm::StringMap<FileData, llvm::BumpPtrAllocator> StatCalls;

  void InjectFileOrDirectory(const char *Path, ino_t INode, bool IsFile) {
#ifndef LLVM_ON_WIN32
    SmallString<128> NormalizedPath(Path);
    llvm::sys::path::native(NormalizedPath);
    Path = NormalizedPath.c_str();
#endif

    FileData Data;
    Data.Name = Path;
    Data.Size = 0;
    Data.ModTime = 0;
    Data.UniqueID = llvm::sys::fs::UniqueID(1, INode);
    Data.IsDirectory = !IsFile;
    Data.IsNamedPipe = false;
    Data.InPCH = false;
    StatCalls[Path] = Data;
  }

public:
  // Inject a file with the given inode value to the fake file system.
  void InjectFile(const char *Path, ino_t INode) {
    InjectFileOrDirectory(Path, INode, /*IsFile=*/true);
  }

  // Inject a directory with the given inode value to the fake file system.
  void InjectDirectory(const char *Path, ino_t INode) {
    InjectFileOrDirectory(Path, INode, /*IsFile=*/false);
  }

  // Implement FileSystemStatCache::getStat().
  LookupResult getStat(StringRef Path, FileData &Data, bool isFile,
                       std::unique_ptr<vfs::File> *F,
                       vfs::FileSystem &FS) override {
#ifndef LLVM_ON_WIN32
    SmallString<128> NormalizedPath(Path);
    llvm::sys::path::native(NormalizedPath);
    Path = NormalizedPath.c_str();
#endif

    if (StatCalls.count(Path) != 0) {
      Data = StatCalls[Path];
      return CacheExists;
    }

    return CacheMissing;  // This means the file/directory doesn't exist.
  }
};

// The test fixture.
class FileManagerTest : public ::testing::Test {
 protected:
  FileManagerTest() : manager(options) {
  }

  FileSystemOptions options;
  FileManager manager;
};

// When a virtual file is added, its getDir() field is set correctly
// (not NULL, correct name).
TEST_F(FileManagerTest, getVirtualFileSetsTheDirFieldCorrectly) {
  const FileEntry *file = manager.getVirtualFile("foo.cpp", 42, 0);
  ASSERT_TRUE(file != nullptr);

  const DirectoryEntry *dir = file->getDir();
  ASSERT_TRUE(dir != nullptr);
  EXPECT_EQ(".", dir->getName());

  file = manager.getVirtualFile("x/y/z.cpp", 42, 0);
  ASSERT_TRUE(file != nullptr);

  dir = file->getDir();
  ASSERT_TRUE(dir != nullptr);
  EXPECT_EQ("x/y", dir->getName());
}

// Before any virtual file is added, no virtual directory exists.
TEST_F(FileManagerTest, NoVirtualDirectoryExistsBeforeAVirtualFileIsAdded) {
  // An empty FakeStatCache causes all stat calls made by the
  // FileManager to report "file/directory doesn't exist".  This
  // avoids the possibility of the result of this test being affected
  // by what's in the real file system.
  manager.addStatCache(llvm::make_unique<FakeStatCache>());

  EXPECT_EQ(nullptr, manager.getDirectory("virtual/dir/foo"));
  EXPECT_EQ(nullptr, manager.getDirectory("virtual/dir"));
  EXPECT_EQ(nullptr, manager.getDirectory("virtual"));
}

// When a virtual file is added, all of its ancestors should be created.
TEST_F(FileManagerTest, getVirtualFileCreatesDirectoryEntriesForAncestors) {
  // Fake an empty real file system.
  manager.addStatCache(llvm::make_unique<FakeStatCache>());

  manager.getVirtualFile("virtual/dir/bar.h", 100, 0);
  EXPECT_EQ(nullptr, manager.getDirectory("virtual/dir/foo"));

  const DirectoryEntry *dir = manager.getDirectory("virtual/dir");
  ASSERT_TRUE(dir != nullptr);
  EXPECT_EQ("virtual/dir", dir->getName());

  dir = manager.getDirectory("virtual");
  ASSERT_TRUE(dir != nullptr);
  EXPECT_EQ("virtual", dir->getName());
}

// getFile() returns non-NULL if a real file exists at the given path.
TEST_F(FileManagerTest, getFileReturnsValidFileEntryForExistingRealFile) {
  // Inject fake files into the file system.
  auto statCache = llvm::make_unique<FakeStatCache>();
  statCache->InjectDirectory("/tmp", 42);
  statCache->InjectFile("/tmp/test", 43);

#ifdef LLVM_ON_WIN32
  const char *DirName = "C:.";
  const char *FileName = "C:test";
  statCache->InjectDirectory(DirName, 44);
  statCache->InjectFile(FileName, 45);
#endif

  manager.addStatCache(std::move(statCache));

  const FileEntry *file = manager.getFile("/tmp/test");
  ASSERT_TRUE(file != nullptr);
  ASSERT_TRUE(file->isValid());
  EXPECT_EQ("/tmp/test", file->getName());

  const DirectoryEntry *dir = file->getDir();
  ASSERT_TRUE(dir != nullptr);
  EXPECT_EQ("/tmp", dir->getName());

#ifdef LLVM_ON_WIN32
  file = manager.getFile(FileName);
  ASSERT_TRUE(file != NULL);

  dir = file->getDir();
  ASSERT_TRUE(dir != NULL);
  EXPECT_EQ(DirName, dir->getName());
#endif
}

// getFile() returns non-NULL if a virtual file exists at the given path.
TEST_F(FileManagerTest, getFileReturnsValidFileEntryForExistingVirtualFile) {
  // Fake an empty real file system.
  manager.addStatCache(llvm::make_unique<FakeStatCache>());

  manager.getVirtualFile("virtual/dir/bar.h", 100, 0);
  const FileEntry *file = manager.getFile("virtual/dir/bar.h");
  ASSERT_TRUE(file != nullptr);
  ASSERT_TRUE(file->isValid());
  EXPECT_EQ("virtual/dir/bar.h", file->getName());

  const DirectoryEntry *dir = file->getDir();
  ASSERT_TRUE(dir != nullptr);
  EXPECT_EQ("virtual/dir", dir->getName());
}

// getFile() returns different FileEntries for different paths when
// there's no aliasing.
TEST_F(FileManagerTest, getFileReturnsDifferentFileEntriesForDifferentFiles) {
  // Inject two fake files into the file system.  Different inodes
  // mean the files are not symlinked together.
  auto statCache = llvm::make_unique<FakeStatCache>();
  statCache->InjectDirectory(".", 41);
  statCache->InjectFile("foo.cpp", 42);
  statCache->InjectFile("bar.cpp", 43);
  manager.addStatCache(std::move(statCache));

  const FileEntry *fileFoo = manager.getFile("foo.cpp");
  const FileEntry *fileBar = manager.getFile("bar.cpp");
  ASSERT_TRUE(fileFoo != nullptr);
  ASSERT_TRUE(fileFoo->isValid());
  ASSERT_TRUE(fileBar != nullptr);
  ASSERT_TRUE(fileBar->isValid());
  EXPECT_NE(fileFoo, fileBar);
}

// getFile() returns NULL if neither a real file nor a virtual file
// exists at the given path.
TEST_F(FileManagerTest, getFileReturnsNULLForNonexistentFile) {
  // Inject a fake foo.cpp into the file system.
  auto statCache = llvm::make_unique<FakeStatCache>();
  statCache->InjectDirectory(".", 41);
  statCache->InjectFile("foo.cpp", 42);
  manager.addStatCache(std::move(statCache));

  // Create a virtual bar.cpp file.
  manager.getVirtualFile("bar.cpp", 200, 0);

  const FileEntry *file = manager.getFile("xyz.txt");
  EXPECT_EQ(nullptr, file);
}

// The following tests apply to Unix-like system only.

#ifndef LLVM_ON_WIN32

// getFile() returns the same FileEntry for real files that are aliases.
TEST_F(FileManagerTest, getFileReturnsSameFileEntryForAliasedRealFiles) {
  // Inject two real files with the same inode.
  auto statCache = llvm::make_unique<FakeStatCache>();
  statCache->InjectDirectory("abc", 41);
  statCache->InjectFile("abc/foo.cpp", 42);
  statCache->InjectFile("abc/bar.cpp", 42);
  manager.addStatCache(std::move(statCache));

  EXPECT_EQ(manager.getFile("abc/foo.cpp"), manager.getFile("abc/bar.cpp"));
}

// getFile() returns the same FileEntry for virtual files that have
// corresponding real files that are aliases.
TEST_F(FileManagerTest, getFileReturnsSameFileEntryForAliasedVirtualFiles) {
  // Inject two real files with the same inode.
  auto statCache = llvm::make_unique<FakeStatCache>();
  statCache->InjectDirectory("abc", 41);
  statCache->InjectFile("abc/foo.cpp", 42);
  statCache->InjectFile("abc/bar.cpp", 42);
  manager.addStatCache(std::move(statCache));

  ASSERT_TRUE(manager.getVirtualFile("abc/foo.cpp", 100, 0)->isValid());
  ASSERT_TRUE(manager.getVirtualFile("abc/bar.cpp", 200, 0)->isValid());

  EXPECT_EQ(manager.getFile("abc/foo.cpp"), manager.getFile("abc/bar.cpp"));
}

TEST_F(FileManagerTest, addRemoveStatCache) {
  manager.addStatCache(llvm::make_unique<FakeStatCache>());
  auto statCacheOwner = llvm::make_unique<FakeStatCache>();
  auto *statCache = statCacheOwner.get();
  manager.addStatCache(std::move(statCacheOwner));
  manager.addStatCache(llvm::make_unique<FakeStatCache>());
  manager.removeStatCache(statCache);
}

// getFile() Should return the same entry as getVirtualFile if the file actually
// is a virtual file, even if the name is not exactly the same (but is after
// normalisation done by the file system, like on Windows). This can be checked
// here by checkng the size.
TEST_F(FileManagerTest, getVirtualFileWithDifferentName) {
  // Inject fake files into the file system.
  auto statCache = llvm::make_unique<FakeStatCache>();
  statCache->InjectDirectory("c:\\tmp", 42);
  statCache->InjectFile("c:\\tmp\\test", 43);

  manager.addStatCache(std::move(statCache));

  // Inject the virtual file:
  const FileEntry *file1 = manager.getVirtualFile("c:\\tmp\\test", 123, 1);
  ASSERT_TRUE(file1 != nullptr);
  ASSERT_TRUE(file1->isValid());
  EXPECT_EQ(43U, file1->getUniqueID().getFile());
  EXPECT_EQ(123, file1->getSize());

  // Lookup the virtual file with a different name:
  const FileEntry *file2 = manager.getFile("c:/tmp/test", 100, 1);
  ASSERT_TRUE(file2 != nullptr);
  ASSERT_TRUE(file2->isValid());
  // Check that it's the same UFE:
  EXPECT_EQ(file1, file2);
  EXPECT_EQ(43U, file2->getUniqueID().getFile());
  // Check that the contents of the UFE are not overwritten by the entry in the
  // filesystem:
  EXPECT_EQ(123, file2->getSize());
}

#endif  // !LLVM_ON_WIN32

} // anonymous namespace
