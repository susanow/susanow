
Testbed
=======

.. todo::
  書きかけ状態をなんとかする.

jk3

- ip: 10.0.0.3/16
- os: ubuntu16.04LTS
- cpu: Core-i7 6700K (4-phys-cores with HT == 8-lcores)
- ram: 16GB
- nic: intel x540-T2

本計画で使用する,試験用の計算機環境を募集しています.
もしご協力をいただける場合は, @slankdev (twitter)か
slank.dev[at]gmail.comまでご連絡をお願いいたします.

計測結果はなるべく公開させていただきたいと思いますが,
詳しくは相談してください.


.. code-block:: text

  # IP address
  address        name          description
  ----------------------------------------------------
  10.0.0.1/16    router        cisco 1800
  10.0.0.2/16    jk2           raspberry-pi
  10.0.0.3/16    jk3           dpdk-server-x540x4
  10.0.0.4/16    jk4           dpdk-server-x540x3
  10.0.0.5/16    jk5           dpdk-server-x540x3

.. code-block:: text

  世代    番号        ブランド
  1       桁違い      Nehalem
  2       v1(省略)    Sandy-Bridge
  3       v2          Ivy-Bridge
  4       v3          Haswell
  5       v4          Broadwell
  6       v5          Skylake
  7       ??          Kabylake

.. code-block:: text

  型番           price     GHz/TB      n_core/thrd    L3C        その他コメント
  --------------------------------------------------------------------------------
  E5-2687W v3    ---       3.10/3.50   10/20          25MB
  E5-2687W v4    260000    3.00/3.50   12/24          30MB       KamueeZero

  E5-2660  v2    ----      2.20/3.00   10/20          25MB       Lagopus?
  E5-2660  v3    ----      2.60/3.30   10/20          25MB       Lagopus?
  E5-2660  v4    ----      2.00/3.20   14/28          35MB       Lagopus?

  E5-2697  v2    ----      2.70/3.50   12/24          30MB       Lagopus

  E5-2630  v3    76000     2.40/3.20   08/16          20MB
  E5-2630  v4    76000     2.20/3.10   10/20          25MB


