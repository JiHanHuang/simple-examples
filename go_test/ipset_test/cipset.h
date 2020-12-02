#ifndef __cipset
#define __cipset

/* Setname X which can be used in "ipset list X". */
#define SETNAME_IPV4 "ipset-ipv4"
#define SETNAME_IPV6 "ipset-ipv6"

struct ipset_state;
struct address;

struct ipset_state *ipset_init(void);
void ipset_add_ip(struct ipset_state *state, struct address *addr);
void ipset_fini(struct ipset_state *state);
void ip2addr(char *ip, struct address *addr);

#endif