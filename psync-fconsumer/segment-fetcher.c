/* Laqin Fan
 */

#include "segment-fetcher.h"

/** initiate the segment fetcher
 */
void
segment_fetcher_init(void* ctx, segment_fetcher_t* sf, ndn_face_intf_t* face, const ndn_name_t* namePrefix, processSegment cb, ndn_ecc_pub_t* pub_key)
{
    sf->cb = cb;
    sf->face = face;
    sf->nextSegNum = 0;
    sf->hasFinal = 1;
    sf->pub_key = pub_key;
    sf->ctx = ctx;
    ndn_interest_init(&sf->interest);
    ndn_name_append_name(&sf->interest.name, namePrefix);

    name_component_t comp;
    comp.type = 33; //sequence number
    comp.value[0] = 0; //start from 0
    comp.size = 1;
    int ret_val = ndn_name_append_component(&sf->interest.name, &comp);
    if (ret_val != 0) {
        printf("error code: %d\n", ret_val);
  }
}

void
segment_fetcher_data(const uint8_t* raw_data, uint32_t data_size, void* userdata)
{
    printf("Receiving data\n");
    char data_buf[1024];
    ndn_data_t data;

    int sigInvalid = ndn_data_tlv_decode_ecdsa_verify(&data, raw_data, data_size, m_segment_fetcher.pub_key);
    if (sigInvalid != 0) {
        printf("segment fetcher data verify: ", sigInvalid);
        return;
    }

    //check if the segment is the last one.
    //
    m_segment_fetcher.hasFinal = name_component_compare(&data.metainfo.final_block_id, &data.name.components[data.name.components_size - 1]);

    bool wantContinue = (*m_segment_fetcher.cb)(&m_segment_fetcher.ctx, &m_segment_fetcher, data.content_value, data.content_size);
    if (wantContinue) {
        m_segment_fetcher.nextSegNum +=1;
        name_component_t comp;
        comp.type = 33; //sequence number type
        comp.value[0] = m_segment_fetcher.nextSegNum; //start from 0
        comp.size = 1;
        ndn_name_append_component(&m_segment_fetcher.interest.name, &comp);

        segment_fetcher_continue(&m_segment_fetcher);
    }
}

void
segment_fetcher_timeout(void* userdata)
{
    (void)userdata;
     printf("On Time Out\n");
}

void
segment_fetcher_start(segment_fetcher_t* sf)
{
    segment_fetcher_continue(sf);
}

void
segment_fetcher_continue(segment_fetcher_t* sf)
{
    if (sf->hasFinal == 0) {
        return;
    }
    // TODO express Interest
    ndn_encoder_t encoder;
    uint8_t buf[1024];

    ndn_interest_set_CanBePrefix(&sf->interest, false);
    encoder_init(&encoder, buf, 1024);
    ndn_interest_tlv_encode(&encoder, &sf->interest);

    int ret = ndn_forwarder_express_interest(encoder.output_value, encoder.offset, segment_fetcher_data, segment_fetcher_timeout, NULL);

    if (ret != 0) {
        printf("Fail to send out segment Interest. Error Code: %d\n", ret);
        return;
    }
}
