#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <libipset/types.h>
#include <libipset/session.h>
#include <libipset/data.h>
#include <netinet/in.h>   
#include <arpa/inet.h>
#include "cipset.h"

struct ipset_state {
    struct ipset_session *session;
};

struct address {
    int family;
    union {
        struct in_addr ip4_addr;
        struct in6_addr ip6_addr;
    };
};

static bool try_ipset_cmd(struct ipset_session *session, enum ipset_cmd cmd,
		        const char *setname, int family, const void *addr)
{
    ipset_session_data_set(session, IPSET_SETNAME, setname);
    if (!ipset_type_get(session, cmd)) {
	        fprintf(stderr, "Cannot find ipset %s: %s\n", setname,
					                ipset_session_error(session));
	        return false;
	    }
    ipset_session_data_set(session, IPSET_OPT_FAMILY, &family);
    ipset_session_data_set(session, IPSET_OPT_IP, addr);

    if (ipset_cmd(session, cmd, /*lineno*/ 0)) {
	        fprintf(stderr, "Failed to add to set %s: %s\n", setname,
					                ipset_session_error(session));
	        return false;
	    }
    return true;
}

static bool try_ipset_create(struct ipset_session *session, const char *setname,
		        const char *typename, int family)
{
    const struct ipset_type *type;
    uint32_t timeout;

    ipset_session_data_set(session, IPSET_SETNAME, setname);
    ipset_session_data_set(session, IPSET_OPT_TYPENAME, typename);
    type = ipset_type_get(session, IPSET_CMD_CREATE);
    if (type == NULL) {
	        fprintf(stderr, "Cannot find ipset type %s: %s\n", typename,
					                ipset_session_error(session));
	        return false;
	    }

    timeout = 0; /* timeout support, but default to infinity */
    ipset_session_data_set(session, IPSET_OPT_TIMEOUT, &timeout);
    ipset_session_data_set(session, IPSET_OPT_TYPE, type);
    ipset_session_data_set(session, IPSET_OPT_FAMILY, &family);

    if (ipset_cmd(session, IPSET_CMD_CREATE, /*lineno*/ 0)) {
	        fprintf(stderr, "Failed to create ipset %s: %s\n", setname,
					                ipset_session_error(session));
	        return false;
	    }
    return true;
}

struct ipset_state *ipset_init(void)
{
    struct ipset_state *state;

    state = malloc(sizeof(*state));
    if (!state)
        return NULL;

    ipset_load_types();

    state->session = ipset_session_init(printf);
    if (!state->session) {
	        fprintf(stderr, "Cannot initialize ipset session.\n");
	        goto err_session;
	    }

    /* Return success on attempts to create a compatible ipset or attempts to
	 *      * add an existing rule. */
    ipset_envopt_parse(state->session, IPSET_ENV_EXIST, NULL);

    if (!try_ipset_create(state->session, SETNAME_IPV4, "hash:ip", NFPROTO_IPV4))
        goto err_set;
    if (!try_ipset_create(state->session, SETNAME_IPV6, "hash:ip", NFPROTO_IPV6))
        goto err_set;

    return state;

err_set:
err_session:
    ipset_session_fini(state->session);
    free(state);
    return NULL;
}

void ipset_add_ip(struct ipset_state *state, struct address *addr)
{
    struct ipset_session *session = state->session;

    switch (addr->family) {
	    case AF_INET:
	        try_ipset_cmd(session, IPSET_CMD_ADD, SETNAME_IPV4, NFPROTO_IPV4, &addr->ip4_addr);
	        break;
	    case AF_INET6:
	        try_ipset_cmd(session, IPSET_CMD_ADD, SETNAME_IPV6, NFPROTO_IPV6, &addr->ip6_addr);
	        break;
	    default:
	        fprintf(stderr, "Unrecognized address family 0x%04x\n", addr->family);
	        return;
	    }
    ipset_session_report_reset(session);
}

void ipset_fini(struct ipset_state *state)
{
    ipset_session_fini(state->session);
    free(state);
}

void ip2addr(char *ip, struct address *addr)
{
    addr->family = AF_INET;
    inet_aton(ip, &(addr->ip4_addr));
}

//for test
int main()
{
    struct address addr;
    ip2addr("1.1.1.1", &addr);
    struct ipset_state *state = ipset_init();
    ipset_add_ip(state, &addr);
    ipset_fini(state);
	return 0;
}
