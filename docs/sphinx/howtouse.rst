
How to Use Susanow
==================

Install
^^^^^^^^
It requres git to install susanow. If you didn't install git,
please install git `following this <https://git-scm.com/book/en/v2/Getting-Started-Installing-Git>`_ .

Then, run below's command to install. It is too easy so it will be finish in about 30 seconds.

.. code-block:: shell

    $ git clone https://github.com/susanow/susanow.git



Hello World
^^^^^^^^^^^^
HelloWorld example in susanow/example/helloworld is very easy example to use Susanow.
It works only build test with Susanow Lib.

Please Run belows to test build.

.. code-block:: shell

    $ cd susanow/example/helloworld
    $ vi Makefile   // set SSN as susanow-installed-path
    $ make
    $ sudo ./a.out



Programing with SusanowLib
^^^^^^^^^^^^^^^^^^^^^^^^^^

You must add some compile/link options to your build system.
It is too easy so you begin to use susanow so quickly.

All tasks is following below.

 - add CXXFLAGS as compiler option
 - add LDFLAGS as linker option
 - add INCLUDES as include directory

susanow/example/helloworld is good example to begin susanow using.
Sample Makefile is below.

.. code-block:: Makefile

    SSN := /home/slank/git/susanow
    include $(SSN)/mk/vars.mk

    all:
        g++ $(CXXFLAGS) main.cc $(INCLUDES) $(LDFLAGS)



API documentation
^^^^^^^^^^^^^^^^^^

More usage about Susanow lib, please see
`API Documentation  <https://susanow.github.io/api/>`_ .
That is generated thanks to `doxygen <http://www.stack.nl/~dimitri/doxygen/>`_ .
