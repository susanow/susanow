
.. Susanow documentation master file, created by
   sphinx-quickstart on Fri Oct 13 12:33:10 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

自動スレッド最適化
===================================


Dynamic Re CPU Pinning (DRCP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Dynamic Re CPU Pinning is the most important feature of Susanow.
It optimizes the pattern of CPU Pinning themselves, while runnning.

For example, system uses very simple CPU pinning pattern like below.

.. image:: ./cpupin0.png
.. image:: ./cpupin1.png


Memo
^^^^

 - Hotspot is moved by traffic pattern.
 - Reseach on Tuning Algorithm
 - It it possible to separate the specification that user
   want to implement and the high performance
 - This idea and Manycore are good chemistry


