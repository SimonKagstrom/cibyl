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
from threading import Thread
from SocketServer import StreamRequestHandler, TCPServer
import sys
sys.path.append("../..") # WHY, WHY, WHY is this needed????
from Cibyl import elf, config
import packets, gdbmips, jdb
import pexpect

def calcChecksum(s):
	csum = 0
	for c in s:
		csum += ord(c)
	return csum % 256

class GdbConnection:
	def __init__(self, wfile):
		self.wfile = wfile

	def writePacket(self, s):
		csum = calcChecksum(s)
		pkg = "$%s#%02x" % (s, csum)
		if config.verbose and s != "":
			print "SND:", pkg
		self.wfile.write(pkg)

	def ack(self):
		self.wfile.write("+")

	def nak(self):
		self.wfile.write("-")

class JdbThread(Thread):
	def __init__(self, gdb):
		self.gdb = gdb
		Thread.__init__(self)

	def run(self):
		packets.run(self.gdb)

class GdbServerRequestHandler(StreamRequestHandler):
	def readChecksum(self):
		a = self.rfile.read(2)
		return int(a,16)

	def readPacket(self, gdb):
		cur = ""
		c = self.rfile.read(1)

		# Interrupt!
		if ord(c) == 3:
			jdb.doInterrupt()
			return
		# Handle acks from gdb here
		assert(c != "-")
		if c == "+":
			return
		while c != "$":
			c = self.rfile.read(1)
		while c != "#":
			c = self.rfile.read(1)
			cur = cur + c
		packet = cur[:-1]
		csum = calcChecksum(packet)
		read_csum = self.readChecksum()
		if csum != read_csum:
			gdb.nak()
			print cur, "has wrong checksum:", csum, read_csum
		else:
			gdb.ack()
		if config.verbose:
			print "PKG:", cur

		# The response will be sent by the encoder
		packets.enqueue(packet)
		print "Returning"

	def handle(self):
		gdb = GdbConnection(self.wfile)

		thr = JdbThread(gdb)
		thr.setDaemon(True)
		thr.start()

		while True:
			try:
				self.readPacket(gdb)
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
