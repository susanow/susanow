


#include <vnfs/acl.h>
#include <dpdk/dpdk.h>
#include <vector>
#include <stdint.h>
#include <stddef.h>
#include <slankdev/exception.h>
#include <slankdev/string.h>
#include <slankdev/hexdump.h>

struct ipv4_5tuple {
  uint8_t pad0;
  uint8_t proto;
  uint8_t pad1;
  uint8_t pad2;
  uint32_t ip_src;
  uint32_t ip_dst;
  uint32_t port_src;
  uint32_t port_dst;
};

struct rte_acl_field_def ipv4_defs[] = {
  /* first input field - always one byte long */
  {
    type : RTE_ACL_FIELD_TYPE_BITMASK,
    size : sizeof (uint8_t),
    field_index : 0,
    input_index : 0,
    offset : offsetof (struct ipv4_5tuple, proto),
  },

  /* next input field (IPv4 source address) - 4 consecutive bytes */
  {
    type : RTE_ACL_FIELD_TYPE_MASK,
    size : sizeof (uint32_t),
    field_index : 1,
    input_index : 1,
    offset : offsetof (struct ipv4_5tuple, ip_src),
  },

  /* next input field (IPv4 destination address) - 4 consecutive bytes */
  {
    type : RTE_ACL_FIELD_TYPE_MASK,
    size : sizeof (uint32_t),
    field_index : 2,
    input_index : 2,
    offset : offsetof (struct ipv4_5tuple, ip_dst),
  },

  /*
   * Next 2 fields (src & dst ports) form 4 consecutive bytes.
   * They share the same input index.
   */
  {
    type : RTE_ACL_FIELD_TYPE_RANGE,
    size : sizeof (uint16_t),
    field_index : 3,
    input_index : 3,
    offset : offsetof (struct ipv4_5tuple, port_src),
  },

  {
    type : RTE_ACL_FIELD_TYPE_RANGE,
    size : sizeof (uint16_t),
    field_index : 4,
    input_index : 3,
    offset : offsetof (struct ipv4_5tuple, port_dst),
  },
};


RTE_ACL_RULE_DEF(acl_ipv4_rule, RTE_DIM(ipv4_defs));
struct rte_acl_param prm = {
  name : "ACL_example",
  socket_id : SOCKET_ID_ANY,
  rule_size : RTE_ACL_RULE_SZ(RTE_DIM(ipv4_defs)),
  max_rule_num : 100000,
};
std::vector<struct acl_ipv4_rule> ipv4_acl_rules;

acl_5tuple::acl_5tuple() : acx(nullptr)
{
  printf("ACL Initilization\n");
  acx = rte_acl_create(&prm);
  if (!acx) throw slankdev::exception("rte_acl_create");
}

acl_5tuple::~acl_5tuple()
{
  rte_acl_reset(acx);
  rte_acl_free(acx);
}

void acl_5tuple::build()
{
  rte_acl_reset_rules(acx);

  struct rte_acl_rule* acl_rules = (struct rte_acl_rule*)(ipv4_acl_rules.data());
  int ret = rte_acl_add_rules(acx, acl_rules, ipv4_acl_rules.size());
  if (ret < 0) throw slankdev::exception("rte_acl_add_rules");

  struct rte_acl_config cfg;
  memset(&cfg, 0x00, sizeof(struct rte_acl_config));
  cfg.max_size = 0x0800000;
  cfg.num_categories = 1;
  cfg.num_fields = RTE_DIM(ipv4_defs);
  memcpy(&cfg.defs, ipv4_defs, sizeof(ipv4_defs));

  dpdk::rte_acl_build(acx, &cfg);
  rte_acl_dump(acx);
}

void acl_5tuple::add_rule(
    uint32_t userdata, uint32_t category_mask, uint32_t priority,
    uint8_t proto,
    uint32_t src_addr, uint8_t src_addr_pref,
    uint32_t dst_addr, uint8_t dst_addr_pref,
    uint16_t src_port_min, uint16_t src_port_max,
    uint16_t dst_port_min, uint16_t dst_port_max)
{
  struct acl_ipv4_rule rule;
  memset(&rule, 0x00, sizeof(rule));
  rule.data.userdata      = userdata;
  rule.data.category_mask = category_mask;
  rule.data.priority      = priority;

  /*
   * IMPORTANT!!!!
   * depending on field type:
   * - mask    ->  1.2.3.4/32    value=0x1020304,     mask_range=32,
   * - range   ->  0:65535       value=0,             mask_range=65535,
   * - bitmask ->  0x06/0xff     value=6,             mask_range=0xff.
   */
  rule.field[0].value.u8       = proto;
  rule.field[0].mask_range.u8  = 0xff;
  rule.field[1].value.u32      = src_addr;
  rule.field[1].mask_range.u32 = src_addr_pref;
  rule.field[2].value.u32      = dst_addr;
  rule.field[2].mask_range.u32 = dst_addr_pref;
  rule.field[3].value.u16      = src_port_min;
  rule.field[3].mask_range.u16 = src_port_max;
  rule.field[4].value.u16      = dst_port_min;
  rule.field[4].mask_range.u16 = dst_port_max;

  ipv4_acl_rules.push_back(rule);
}

bool acl_5tuple::packet_filter_pass(rte_mbuf* mbuf)
{
  uint8_t* ptr = rte_pktmbuf_mtod_offset(mbuf, uint8_t*, 14+8);
  const uint8_t* data[] = {ptr};
  const uint32_t n_data = 1;
  const uint32_t n_cate = 1;
  uint32_t results[] = {3};
  rte_acl_classify(acx, data, results, n_data , n_cate);

#if 0
  printf("  ");
  for (uint32_t i=0; i<1; i++) {
    printf("%u,", results[i]);
  } printf("  ");
  fflush(stdout);
#endif

  return (results[0] == 0);
}


/*
 * This function does not consider that
 * argument 'n_mbufs' is bigger than MAX_BURSTS.
 */
size_t acl_5tuple::packet_filter_bulk(rte_mbuf** mbufs, size_t n_mbufs,
    rte_mbuf** pass_mbufs, rte_mbuf** ejct_mbufs)
{
  constexpr size_t MAX_BURSTS = 32;
  const uint8_t* data[MAX_BURSTS] = {nullptr};
  uint32_t results[MAX_BURSTS] = {0};
  const uint32_t n_data = n_mbufs;
  const uint32_t n_cate = 1;

  for (size_t i=0; i<n_mbufs; i++) {
    data[i] = rte_pktmbuf_mtod_offset(mbufs[i], uint8_t*, 14+8);
  }

  size_t pass_idx = 0;
  size_t ejct_idx = 0;
  rte_acl_classify_alg(acx, data, results, n_data, n_cate, RTE_ACL_CLASSIFY_AVX2);
  for (size_t i=0; i<n_mbufs; i++) {
    if (results[i] == 0) pass_mbufs[pass_idx++] = mbufs[i];
    else                 ejct_mbufs[ejct_idx++] = mbufs[i];
  }

#if 0
  printf("  ");
  for (uint32_t i=0; i<n_data; i++) {
    printf("%u,", results[i]);
  } printf("  ");
  fflush(stdout);
#endif

  return pass_idx;
}

