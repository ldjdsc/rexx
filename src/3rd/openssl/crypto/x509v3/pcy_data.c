
#include "cryptlib.h"
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include "pcy_int.h"

/* Policy Node routines */

void policy_data_free(X509_POLICY_DATA *data)
{
    ASN1_OBJECT_free(data->valid_policy);
    /* Don't free qualifiers if shared */
    if (!(data->flags & POLICY_DATA_FLAG_SHARED_QUALIFIERS))
        sk_POLICYQUALINFO_pop_free(data->qualifier_set, POLICYQUALINFO_free);
    sk_ASN1_OBJECT_pop_free(data->expected_policy_set, ASN1_OBJECT_free);
    OPENSSL_free(data);
}

/*
 * Create a data based on an existing policy. If 'id' is NULL use the oid in
 * the policy, otherwise use 'id'. This behaviour covers the two types of
 * data in RFC3280: data with from a CertificatePolcies extension and
 * additional data with just the qualifiers of anyPolicy and ID from another
 * source.
 */

X509_POLICY_DATA *policy_data_new(POLICYINFO *policy,
                                  const ASN1_OBJECT *cid, int crit)
{
    X509_POLICY_DATA *ret;
    ASN1_OBJECT *id;
    if (!policy && !cid)
        return NULL;
    if (cid) {
        id = OBJ_dup(cid);
        if (!id)
            return NULL;
    } else
        id = NULL;
    ret = OPENSSL_malloc(sizeof(X509_POLICY_DATA));
    if (!ret)
        return NULL;
    ret->expected_policy_set = sk_ASN1_OBJECT_new_null();
    if (!ret->expected_policy_set) {
        OPENSSL_free(ret);
        if (id)
            ASN1_OBJECT_free(id);
        return NULL;
    }

    if (crit)
        ret->flags = POLICY_DATA_FLAG_CRITICAL;
    else
        ret->flags = 0;

    if (id)
        ret->valid_policy = id;
    else {
        ret->valid_policy = policy->policyid;
        policy->policyid = NULL;
    }

    if (policy) {
        ret->qualifier_set = policy->qualifiers;
        policy->qualifiers = NULL;
    } else
        ret->qualifier_set = NULL;

    return ret;
}
