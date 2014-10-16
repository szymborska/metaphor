.. metaphor documentation master file, created by
   sphinx-quickstart on Mon Oct 13 17:34:09 2014.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to metaphor's documentation!
====================================

Contents:

.. toctree::
   :maxdepth: 2

==================
Synopsis
==================
metaphor program [arguments]

Alternatively:

@ mysql program [arguments]

==================
Description
==================
Metaphor allows users to use familiar UNIX programs like top, ls, chmod, grep and more to interact with *unfamiliar* programs. As an example, the metaphor command:

@ mysql ps aux

will display the queries currently running on the MySQL server:

USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root     30738  0.0  0.0      0     0 ?        S+   Sep19   0:00 [init]
root       382  0.0  0.0      0     0 ?        S+   Sep19   0:00 [show processlis]

Metahor works by enabling UNIX programs to run on top of one another. A UNIX program running on a metaphor-enabled program will have its system calls redirected to application-specific replacements. To see more information on adding support to a program for metaphor, please consult the metaphor documentation at http://metaphor.io.

==================
Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

