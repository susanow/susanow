

Extend Susanow
==============

Rules
^^^^^

 - don't use [] operator, use .at().


Add New Queue
^^^^^^^^^^^^^^

You can add and select original Queue implementaion. The all tasks are following below.

 - Define OriginalQueue class inherrit Ring_interface class.
 - Implement all pure virtual functions.
 - Using OriginalQueue class at Template specialization.

Example: Ring_stdqueue is good sample to add new queue class. on susanow/lib/include/ssnlib_ring.h

At first, define and implement originalclass ad Ring_stdqueue. it uses std::queue class.
It is thread unsafe so we must be exclusive with std::mutex.


.. code-block:: cpp

    class Ring_stdqueue : public Ring_interface {
        std::queue<rte_mbuf*> queue_;
        mutable std::mutex* m;
        using auto_lock = std::lock_guard<std::mutex>;
    public:
        Ring_stdqueue(size_t, size_t, size_t, const char*) { m = new std::mutex; }
        virtual ~Ring_stdqueue() { delete(m); }
        Ring_stdqueue(const Ring_stdqueue&) { m = new std::mutex; }
        Ring_stdqueue(Ring_stdqueue&&) = default;
        void push_bulk(rte_mbuf** obj_table, size_t n) override
        {
            auto_lock lg(*m);
            for (size_t i=0; i<n; i++) {
                queue_.push(obj_table[i]);
            }
        }
        bool pop_bulk(rte_mbuf** obj_table, size_t n) override
        {
            auto_lock lg(*m);
            if (queue_.size() < n)
                return false;

            for (size_t i=0; i<n; i++) {
                obj_table[i] = queue_.front();
                queue_.pop();
            }
            return true;
        }
        size_t count() const override { auto_lock lg(*m); return queue_.size(); }
        size_t size()  const override { auto_lock lg(*m); return queue_.size(); }
        bool   empty() const override { auto_lock lg(*m); return queue_.empty(); }
    };

Code is enough. Finaly, specialize template.

.. code-block:: cpp

    using Rxq    = ssnlib::Rxq_interface<ssnlib::Ring_dpdk>;
    using Txq    = ssnlib::Txq_interface<ssnlib::Ring_dpdk>;
    using Port   = ssnlib::Port_interface<Rxq, Txq>;
    using Cpu    = ssnlib::Cpu_interface;
    using System = ssnlib::System_interface<Cpu, Port>;

    System sys(argc, argv);

This sample implementation uses std::queue and std::mutex, so it's slower than Ring_dpdk,
default Ring_interface implementation.
If you want add/test new queue algorithm/implementation, following these to add new class.


