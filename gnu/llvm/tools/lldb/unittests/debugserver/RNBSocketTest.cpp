//===-- RNBSocketTest.cpp ---------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"

#include <arpa/inet.h>
#include <sys/sysctl.h>
#include <unistd.h>

#include "RNBDefs.h"
#include "RNBSocket.h"
#include "lldb/Host/Socket.h"
#include "lldb/Host/StringConvert.h"
#include "lldb/Host/common/TCPSocket.h"

using namespace lldb_private;

std::string hello = "Hello, world!";
std::string goodbye = "Goodbye!";

static void ServerCallbackv4(const void *baton, in_port_t port) {
  auto child_pid = fork();
  if (child_pid == 0) {
    Socket *client_socket;
    char addr_buffer[256];
    sprintf(addr_buffer, "%s:%d", baton, port);
    Status err = Socket::TcpConnect(addr_buffer, false, client_socket);
    if (err.Fail())
      abort();
    char buffer[32];
    size_t read_size = 32;
    err = client_socket->Read((void *)&buffer[0], read_size);
    if (err.Fail())
      abort();
    std::string Recv(&buffer[0], read_size);
    if (Recv != hello)
      abort();
    size_t write_size = goodbye.length();
    err = client_socket->Write(goodbye.c_str(), write_size);
    if (err.Fail())
      abort();
    if (write_size != goodbye.length())
      abort();
    delete client_socket;
    exit(0);
  }
}

void TestSocketListen(const char *addr) {
  // Skip IPv6 tests if there isn't a valid interafce
  auto addresses = lldb_private::SocketAddress::GetAddressInfo(
      addr, NULL, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
  if (addresses.size() == 0)
    return;

  char addr_wrap[256];
  if (addresses.front().GetFamily() == AF_INET6)
    sprintf(addr_wrap, "[%s]", addr);
  else
    sprintf(addr_wrap, "%s", addr);

  RNBSocket server_socket;
  auto result =
      server_socket.Listen(addr, 0, ServerCallbackv4, (const void *)addr_wrap);
  ASSERT_TRUE(result == rnb_success);
  result = server_socket.Write(hello.c_str(), hello.length());
  ASSERT_TRUE(result == rnb_success);
  std::string bye;
  result = server_socket.Read(bye);
  ASSERT_TRUE(result == rnb_success);
  ASSERT_EQ(bye, goodbye);

  int exit_status;
  wait(&exit_status);
  ASSERT_EQ(exit_status, 0);
}

TEST(RNBSocket, LoopBackListenIPv4) { TestSocketListen("127.0.0.1"); }

TEST(RNBSocket, LoopBackListenIPv6) { TestSocketListen("::1"); }

TEST(RNBSocket, AnyListen) { TestSocketListen("*"); }

void TestSocketConnect(const char *addr) {
  // Skip IPv6 tests if there isn't a valid interafce
  auto addresses = lldb_private::SocketAddress::GetAddressInfo(
      addr, NULL, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
  if (addresses.size() == 0)
    return;

  char addr_wrap[256];
  if (addresses.front().GetFamily() == AF_INET6)
    sprintf(addr_wrap, "[%s]:0", addr);
  else
    sprintf(addr_wrap, "%s:0", addr);

  Socket *server_socket;
  Predicate<uint16_t> port_predicate;
  port_predicate.SetValue(0, eBroadcastNever);
  Status err =
      Socket::TcpListen(addr_wrap, false, server_socket, &port_predicate);
  ASSERT_FALSE(err.Fail());

  auto port = ((TCPSocket *)server_socket)->GetLocalPortNumber();
  auto child_pid = fork();
  if (child_pid != 0) {
    RNBSocket client_socket;
    auto result = client_socket.Connect(addr, port);
    ASSERT_TRUE(result == rnb_success);
    result = client_socket.Write(hello.c_str(), hello.length());
    ASSERT_TRUE(result == rnb_success);
    std::string bye;
    result = client_socket.Read(bye);
    ASSERT_TRUE(result == rnb_success);
    ASSERT_EQ(bye, goodbye);
  } else {
    Socket *connected_socket;
    err = server_socket->Accept(connected_socket);
    if (err.Fail()) {
      llvm::errs() << err.AsCString();
      abort();
    }
    char buffer[32];
    size_t read_size = 32;
    err = connected_socket->Read((void *)&buffer[0], read_size);
    if (err.Fail()) {
      llvm::errs() << err.AsCString();
      abort();
    }
    std::string Recv(&buffer[0], read_size);
    if (Recv != hello) {
      llvm::errs() << err.AsCString();
      abort();
    }
    size_t write_size = goodbye.length();
    err = connected_socket->Write(goodbye.c_str(), write_size);
    if (err.Fail()) {
      llvm::errs() << err.AsCString();
      abort();
    }
    if (write_size != goodbye.length()) {
      llvm::errs() << err.AsCString();
      abort();
    }
    exit(0);
  }
  int exit_status;
  wait(&exit_status);
  ASSERT_EQ(exit_status, 0);
}

TEST(RNBSocket, LoopBackConnectIPv4) { TestSocketConnect("127.0.0.1"); }

TEST(RNBSocket, LoopBackConnectIPv6) { TestSocketConnect("::1"); }
