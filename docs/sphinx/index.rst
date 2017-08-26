
.. Susanow documentation master file, created by
   sphinx-quickstart on Fri Feb  3 13:01:27 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.


Susanow
=======

Susanow is a next generation platform to perform High Performance Networking.
Its backend is DPDK, provides Tuning-less platform.

Susanow (須佐之男 (スサノオ)?, romanized as Susano-o, Susa-no-O, Susano'o,
and Susanowo), also known as Takehaya Susanoo-no-Mikoto (建速須佐之男命?)
and Kumano Ketsumiko no kami at Kumano shrine,
is the Shinto god of the sea and storms.
He is also considered to be ruler of Neno-Katasu-Kuni
(now in Yasugi-shi, Shimane-ken). He is married to Kushinadahime.

Why we named this platform, Susanow is influenced by Kamuee0 that is
a high speed software router developed by Head of Technology Development team
in NTT-Comunications.

More detail, please see :doc:`introduction` .



Concept/Implementation
^^^^^^^^^^^^^^^^^^^^^^

 - Tuning-Less
 - Safe Implementation
 - Wirerate (128byte packet)
 - Extensible
 - Using useful components on C++11 (move semantics/smart ptr)
 - All module are included that are needed HighPerformanceNetworking

More detail, please see :doc:`feature` .
If you want to know implementation susanow, please see :doc:`implement` .
If you want to extend susanow, please see :doc:`extend` .



Benchmark
^^^^^^^^^

We calc benchmark on PC/Workstation. Now, Susanow support 10GbE.
More detail, please see :doc:`benchmark` .



Documentation Contens
^^^^^^^^^^^^^^^^^^^^^

.. only:: html

   See the sections below for more details.



.. toctree::
  :maxdepth: 1

  introduction.rst
  feature.rst
  howtouse.rst
  implement.rst
  benchmark.rst
  license.rst
  drcp.rst
  glossary.rst
  usecase.rst
  apidoc.rst
  extend.rst




