######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      gdbserver.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   GdbServer implementation
##
## $Id:$
##
######################################################################
import socket, os
from SocketServer import StreamRequestHandler, TCPServer
import sys
sys.path.append("../..") # WHY, WHY, WHY is this needed????
from Cibyl import elf, config
import packets, gdbmips, jdb
import pexpect

class GdbServerRequestHandler(StreamRequestHandler):
    def readChecksum(self):
        a = self.rfile.read(2)
        return int(a,16)

    def calcChecksum(self, s):
        csum = 0
        for c in s:
            csum += ord(c)
        return csum % 256

    def writePacket(self, s):
        csum = self.calcChecksum(s)
        pkg = "$%s#%02x" % (s, csum)
        if config.verbose and s != "":
            print "SND:", pkg
        self.wfile.write(pkg)

    def ack(self):
        self.wfile.write("+")

    def nak(self):
        self.wfile.write("-")

    def readPacket(self):
        cur = ""
        c = self.rfile.read(1)
        while c != "$":
            c = self.rfile.read(1)
        while c != "#":
            c = self.rfile.read(1)
            cur = cur + c
        packet = cur[:-1]
        csum = self.calcChecksum(packet)
        read_csum = self.readChecksum()
        if csum != read_csum:
            self.nak()
            print cur, "has wrong checksum:", csum, read_csum
        else:
            self.ack()
        if config.verbose:
            print "PKG:", cur

        response = packets.handle(packet)
        if response == None:
            # Some error - return
            self.nak()
            return
        self.writePacket(response)
        # Read response
        c = self.rfile.read(1)
        if c == "-":
            print "NAK: Something is wrong with the last packet", c
            # Resend once
            self.writePacket(response)
            return

    def handle(self):
        while True:
            try:
                self.readPacket()
            except packets.PacketException, inst:
                print "GDB connection lost, exiting..."
                raise inst

class GdbServer(TCPServer):
    def server_bind(self):
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.bind(self.server_address)


def init(elf_file, port, jdb_args):
    gdbmips.elf = elf.Elf(elf_file)
    gdbmips.textSection = gdbmips.elf.getSectionContents(".text")

    print "Starting JDB..."
    jdb.process = pexpect.spawn("jdb %s" % (jdb_args))
    jdb.init()
    print "Done. The server is listening on port", port

    s = GdbServer(("", port), GdbServerRequestHandler)
    s.serve_forever()

if __name__ == "__main__":
    init("/home/ska/projects/private/games/cibyl/trunk/examples/hello-world/c/program")
