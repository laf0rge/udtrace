udtrace - Unix Domain socket tracing
====================================

This is a LD_PRELOAD wrapper library which can be used to trace the data
sent and/or received via unix domain sockets.

Unlike IP based communication that can be captured/traced with pcap
programs like tcpdump or wireshark, there is no similar mechanism
available for unix domain sockets.

This LD_PRELOAD library intercepts the C library function calls of
dynamically linked programs.  It will detect all file descriptors
representing unix domain sockets and will then print traces of all
data sent/received via the socket.

Usage
-----

Simply build **libudtrace.so** using the **make** command, and then
start your to-be-traced program with

  LD_PRELOAD=libudtrace.os

e.g.

  LD_PRELOAD=libudtrace.so systemctl status

which will produce output like this:

  >>> UDTRACE: Unix Domain Socket Trace initialized (TITAN support DISABLED)
  >>> UDTRACE: Adding FD 4
  >>> UDTRACE: connect(4, "/run/dbus/system_bus_socket")
  4 sendmsg W 00415554482045585445524e414c20
  4 sendmsg W 3331333033303330
  4 sendmsg W 0d0a4e45474f54494154455f554e49585f46440d0a424547494e0d0a
  [...]

Output Format
-------------

Currently, **udtrace** will produc the following output:


At time a FD for a unix domain socket is created:

  >>> UDTRACE: Adding FD 8

At time a FD for a unix domain socket is closed:

  >>> UDTRACE: Removing FD 8

At time a FD for a unix domain socket is bound or connected:

  >>> UDTRACE: connect(9, "/tmp/mncc")

When data is read from the socket:

  9 read R 00040000050000004403000008000000680000001c0300002c03000000000000

When data is written to the socket:

  9 write W 00040000050000004403000008000000680000001c0300002c03000000000000

Where
 * *9* is the file dsecriptor on which the event happened
 * *read/write* is the name of the syscall, could e.g. also be sendmsg / readv / etc.
 * *R|W* is Read / Write (from the process point of view)
 * followed by a hex-dump of the raw data.  Only data successfully
   written (or read) will be printed, not the entire buffer passed to
   the syscall.  The rationale is to only print data  that was actually
   sent to or received from the socket.

TITAN decoder support
---------------------

Getting hex-dumps is nice and fine, but normally one wants to have a
more detailed decode of the data that is being passed on the socket.

For TCP based protocols, there is wireshark.  But most protocols on unix
domain sockets don't follow inter-operable / public standards, so even
if one was to pass the traces into wireshark somehow, there would be no
decoder.

In the [Osmocom project](https://osmocom.org/), we already had some type
definitions and decoders for our protocols written in the TTCN-3
programming language, using [Eclipse TITAN](https://projects.eclipse.org/projects/tools.titan).
In order to build those decoders fro MNCC and PCUIF, please use

  make ENABLE_TITAN=1

when building the code.

Please note that this introduces a run-time dependency to
libttcn3-dynamic.so, which is (at least on Debian GNU/Linux) not
installed in a default library search path, so you will have to use
something like:

  LD_LIBRARY_PATH=/usr/lib/titan LD_PRELOAD=libudtrace.so systemctl status

