/* Laqin Fan
 */

#ifndef SEGMENT_FETCHER_H
#define SEGMENT_FETCHER_H

#include "../forwarder/face.h"
#include "../encode/interest.h"
#include "../encode/data.h"
#include "../forwarder/callback-funcs.h"
#include "../util/msg-queue.h"
#include "../security/ndn-lite-ecc.h"
#include "../forwarder/forwarder.h"

#ifdef __cplusplus
extern "C" {
#endif

struct segment_fetcher;

/**
 * \retval true continue fetching next segment
 * \retval false pause/stop fetching, may resume with segment_fetcher_continue
 */
typedef bool (*processSegment)(void* ctx, struct segment_fetcher* sf, const uint8_t* payload, size_t size);

typedef struct segment_fetcher {
    ndn_interest_t interest;
    ndn_face_intf_t* face;
    processSegment cb;
    uint64_t nextSegNum;
    int hasFinal; //check if it is the last segment
    ndn_ecc_pub_t* pub_key;
    void* ctx;
} segment_fetcher_t;

static segment_fetcher_t m_segment_fetcher;

/** initiate the segment fetcher
 */
void
segment_fetcher_init(void* ctx, segment_fetcher_t* sf, ndn_face_intf_t* face, const ndn_name_t* namePrefix, processSegment cb, ndn_ecc_pub_t* pub_key);

void
segment_fetcher_data(const uint8_t* raw_data, uint32_t data_size, void* userdata);

void
segment_fetcher_timeout(void* userdata);

void
segment_fetcher_start(segment_fetcher_t* sf);

void
segment_fetcher_continue(segment_fetcher_t* sf);


/*@}*/

#ifdef __cplusplus
}
#endif

#endif // SEGMENT_FETCHER_H
